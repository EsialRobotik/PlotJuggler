#include "AsservStream.h"
#include <QFile>
#include <QMessageBox>
#include <thread>
#include <mutex>
#include <chrono>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <cstdint>
#include <QInputDialog>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <cmath>
#include "ui_asservstreamcontrolpanel.h"
#include "AsservStreamControlPanel.h"

#define ASSERV_FREQ 500.0

AsservStream::AsservStream():_running(false),fd(-1), fdLog(-1)
{
    QStringList  words_list;
    words_list
        << "timestamp"

        <<"speed/right/goal"
        <<"speed/left/goal"
        <<"speed/right/current"
        <<"speed/left/current"
        <<"speed/right/error"
        <<"speed/left/error"
        <<"speed/right/outputConsign"
        <<"speed/left/outputConsign"
        <<"speed/right/output_consign_integrated"
        <<"speed/left/output_consign_integrated"
        <<"speed/right/Kp"
        <<"speed/left/Kp"
        <<"speed/right/Ki"
        <<"speed/left/Ki"


        << "angle_regulator/goal"
        << "angle_regulator/accumulator"
        << "angle_regulator/output"
        << "angle_regulator/limited_output"

        << "distance_regulator/goal"
        << "distance_regulator/accumulator"
        << "distance_regulator/output"
        << "distance_regulator/limited_output"

        << "raw_encoder/right"
        << "raw_encoder/left"

        << "odometry/X"
        << "odometry/Y"
        << "odometry/theta"

        << "commandManager/X"
        << "commandManager/Y"
        ;

   std::lock_guard<std::mutex> lock( mutex() );
    foreach( const QString& name, words_list)
        dataMap().addNumeric(name.toStdString());

    controlPanelWindows = nullptr;
}


bool AsservStream::openPort()
{
    bool ok;

    QDir devDir("/dev/");
    QStringList ttyListAbsolutePath;

    QFileInfoList list = devDir.entryInfoList(QStringList() << "ttyACM*", QDir::System);
    for (int i = list.size() - 1; i >= 0; i--)
        ttyListAbsolutePath << list.at(i).absoluteFilePath();

    QString device = QInputDialog::getItem(nullptr, tr("Which tty use ?"), tr("/dev/tty?"), ttyListAbsolutePath, 0, false, &ok);

    std::string ttyName = device.toStdString();

    if (!ok)
        return false;

    fd = open(ttyName.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        printf("Unable to open %s\n", ttyName.c_str());
        return false;
    }
    else
    {
        printf("Port %s opened\n", ttyName.c_str());
        fdLog = open("commandLog" , O_WRONLY | O_APPEND | O_CREAT, S_IRUSR |S_IWUSR | S_IRGRP|S_IWGRP );
        return true;
    }

}

bool AsservStream::start(QStringList*)
{
    bool ok = openPort();
    bool tryAgain = !ok;
    while (tryAgain)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Unable to open port", "Unable to open port\nTry again?",
                QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            tryAgain = false;
        }
        else
        {
            ok = openPort();
            tryAgain = !ok;
        }
    }

    if (ok)
    {
        std::lock_guard < std::mutex > lock(mutex());
        // Insert dummy Point. it seems that there's a regression on plotjuggler, this dirty hack seems now necessary
        for (auto &it : dataMap().numeric)
        {
            auto &plot = it.second;
            plot.pushBack(PlotData::Point(0, 0));
        }
        _running = true;
        _thread = std::thread([this]()
        {   this->loop();});

        controlPanelWindows = new AsservStreamControlPanel(new Ui_AsservStreamControlPanel, &uartDecoder, fd, fdLog);
        controlPanelWindows->show();

        return true;
    }
    else
    {
        return false;
    }
}

void AsservStream::shutdown()
{
    _running = false;

    if(controlPanelWindows != nullptr)
    	controlPanelWindows->close();

    if(fd != -1)
     close(fd);

    if( fdLog != -1)
        close(fdLog);

    if( _thread.joinable()) _thread.join();
}

bool AsservStream::isRunning() const { return _running; }

AsservStream::~AsservStream()
{
    shutdown();
    if(controlPanelWindows != nullptr)
    	delete(controlPanelWindows);
}

void AsservStream::pushSingleCycle()
{
    UsbStreamSample sample;
    while (uartDecoder.getDecodedSample(&sample))
    {
        std::lock_guard < std::mutex > lock(mutex());

        double timestamp = double(sample.timestamp) * 1.0 / ASSERV_FREQ;

        for (auto &it : dataMap().numeric)
        {
            double value = getValueFromName(it.first, sample);

            if (std::isnan(value))
                value = 0;

            auto &plot = it.second;
            plot.pushBack(PlotData::Point(timestamp, value));
        }
    }
}

void AsservStream::loop()
{
    _running = true;
    while (_running && fd != -1)
    {
        uint8_t read_buffer[sizeof(UsbStreamSample) + 4];
        int bytes_read = 0;
        bytes_read = read(fd, read_buffer, sizeof(read_buffer));

        if (bytes_read < 1)
        {
            emit connectionClosed();
            break;
        }

        if (bytes_read > 0)
            uartDecoder.processBytes(read_buffer, bytes_read);

        pushSingleCycle();
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

}

double AsservStream::getValueFromName(const  std::string &name, UsbStreamSample &sample)
{
    double value = 0;

    if (name == "timestamp")
        value = sample.timestamp;
    else if (name == "speed/right/goal")
        value = sample.value1;
    else if (name == "speed/left/goal")
        value = sample.value4;

    else if (name == "speed/right/current")
        value = sample.value2;
    else if (name == "speed/left/current")
        value = sample.value5;

    else if (name == "speed/right/error")
        value = sample.value1-sample.value2;
    else if (name == "speed/left/error")
        value = sample.value4-sample.value5;

    else if (name == "speed/right/outputConsign")
        value = sample.value3;
    else if (name == "speed/left/outputConsign")
        value = sample.value6;

    else if (name == "speed/right/Kp")
        value = sample.value24;
    else if (name == "speed/left/Kp")
        value = sample.value26;

    else if (name == "speed/right/Ki")
        value = sample.value25;
    else if (name == "speed/left/Ki")
        value = sample.value27;

    else if (name == "speed/right/output_consign_integrated")
        value = sample.value7;
    else if (name == "speed/left/output_consign_integrated")
        value = sample.value8;

    else if (name == "angle_regulator/limited_output")
        value = sample.value9;
    else if (name == "distance_regulator/limited_output")
        value = sample.value10;

    else if (name == "angle_regulator/goal")
        value = sample.value11;
    else if (name == "angle_regulator/accumulator")
        value = sample.value12;
    else if (name == "angle_regulator/output")
        value = sample.value13;

    else if (name == "distance_regulator/goal")
        value = sample.value14;
    else if (name == "distance_regulator/accumulator")
        value = sample.value15;
    else if (name == "distance_regulator/output")
        value = sample.value16;

    else if (name == "raw_encoder/right")
        value = sample.value17;
    else if (name == "raw_encoder/left")
        value = sample.value18;

    else if (name == "odometry/X")
        value = sample.value19;
    else if (name == "odometry/Y")
        value = sample.value20;
    else if (name == "odometry/theta")
        value = sample.value21;

    else if (name == "commandManager/X")
        value = sample.value22;
    else if (name == "commandManager/Y")
        value = sample.value23;

    return value;
}



bool AsservStream::xmlSaveState(QDomDocument &doc, QDomElement &parent_element) const 
{
    return true;
}
    
bool AsservStream::xmlLoadState(const QDomElement &parent_element)
{
    return false;
}
