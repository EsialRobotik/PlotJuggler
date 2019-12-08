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

#define ASSERV_FREQ 500.0

AsservStream::AsservStream():_running(false),fd(-1)
{
    QStringList  words_list;
       words_list
   		<< "timestamp"
		<<"speed_Right_Goal"
		<<"speed_Left_GoalLeft"

		<<"speed_Right"
		<<"speed_Left"

		<<"speed_Right_Output"
		<<"speed_Left_Output"

		<<"speed_Right_OutputIntegrated"
		<<"speed_Left_OutputIntegrated"

		<<"speed_Right_LimitedGoal"
		<<"speed_Left_LimitedGoal"

		<< "angle_Regulator_Goal"
		<< "angle_Regulator_Accumulator"
		<< "angle_Regulator_output"

		<< "distance_Regulator_Goal"
		<< "distance_Regulator_Accumulator"
		<< "distance_Regulator_output"

		<<"raw_encoder_Right"
		<<"raw_encoder_Left"

		<< "odo_X"
		<< "odo_Y"
		<< "odo_theta"

		<< "CommandManager_X"
		<< "CommandManager_Y"
		;




   std::lock_guard<std::mutex> lock( mutex() );
    foreach( const QString& name, words_list)
        dataMap().addNumeric(name.toStdString());
}


bool AsservStream::openPort()
{
	bool ok;

	QDir devDir("/dev/");
	QStringList ttyListAbsolutePath;

	QFileInfoList list = devDir.entryInfoList(QStringList() << "ttyACM*" ,QDir::System);
	for (int i = list.size()-1; i >= 0; i--)
		ttyListAbsolutePath << list.at(i).absoluteFilePath();

	QString device = QInputDialog::getItem(nullptr,
			tr("Which tty use ?"),
			tr("/dev/tty?"),
			ttyListAbsolutePath,
			0, false, &ok);

    std::string ttyName = device.toStdString();


	if(!ok)
		return false;

	fd = open(ttyName.c_str(),O_RDWR | O_NOCTTY);
    if( fd == -1)
    {
        printf("Unable to open %s\n",ttyName.c_str());
        return false;
    }
    else
    {
        printf("Port %s opened\n",ttyName.c_str());
        return true;
    }
}

bool AsservStream::start(QStringList*)
{
	bool ok = openPort();
	bool tryAgain = !ok;
	while(tryAgain)
	{
		QMessageBox::StandardButton reply =
				QMessageBox::question(nullptr, "Unable to open port", "Unable to open port\nTry again?",
						QMessageBox::Yes|QMessageBox::No);
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

	if(ok)
	{
	    std::lock_guard<std::mutex> lock( mutex() );
		// Insert dummy Point. it seems that there's a regression on plotjuggler, this dirty hack seems now necessary
		for (auto& it: dataMap().numeric )
        {
            auto& plot = it.second;
			plot.pushBack( PlotData::Point( 0, 0 ) );
        }   
		_running = true;
		_thread = std::thread([this](){ this->loop();} );
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

    if(fd != -1)
     close(fd);

    if( _thread.joinable()) _thread.join();
}

bool AsservStream::isRunning() const { return _running; }

AsservStream::~AsservStream()
{
    shutdown();
}

void AsservStream::pushSingleCycle()
{
    UsbStreamSample sample;
    while( uartDecoder.getDecodedSample(&sample) )
    {
        std::lock_guard<std::mutex> lock( mutex() );

        double timestamp = double(sample.timestamp) * 1.0/ASSERV_FREQ;

        for (auto& it: dataMap().numeric )
        {
        	double value = getValueFromName(it.first, sample);

        	if(std::isnan(value))
        		value = 0;

            auto& plot = it.second;
			plot.pushBack( PlotData::Point( timestamp, value ) );
        }   
    }
}

void AsservStream::loop()
{
    _running = true;
    while( _running  && fd != -1)
    {
    	uint8_t read_buffer[sizeof(UsbStreamSample)+4];
        int  bytes_read = 0;
        bytes_read = read(fd,read_buffer,sizeof(read_buffer));

        if(bytes_read < 1 )
        {
            emit connectionClosed();
        	break;
        }

        if(bytes_read > 0)
        	uartDecoder.processBytes(read_buffer,bytes_read );

        pushSingleCycle();
        std::this_thread::sleep_for( std::chrono::microseconds(500) );
    }

}

double AsservStream::getValueFromName(const  std::string &name, UsbStreamSample &sample)
{
		double value = 0;

		if( name == "timestamp")
			value = sample.timestamp;
		else if( name == "speed_Right_Goal")
			value = sample.value1;
		else if( name == "speed_Left_GoalLeft")
			value = sample.value4;

		else if( name == "speed_Right")
			value = sample.value2;
		else if( name == "speed_Left")
			value = sample.value5;

		else if( name == "speed_Right_Output")
			value = sample.value3;
		else if( name == "speed_Left_Output")
			value = sample.value6;

		else if( name == "speed_Right_OutputIntegrated")
			value = sample.value7;
		else if( name == "speed_Left_OutputIntegrated")
			value = sample.value8;

		else if( name == "speed_Right_LimitedGoal")
			value = sample.value9;
		else if( name == "speed_Left_LimitedGoal")
			value = sample.value10;


		else if( name == "angle_Regulator_Goal")
			value = sample.value11;
		else if( name == "angle_Regulator_Accumulator")
			value = sample.value12;
		else if( name == "angle_Regulator_output")
				value = sample.value13;

		else if( name == "distance_Regulator_Goal")
			value = sample.value14;
		else if( name == "distance_Regulator_Accumulator")
			value = sample.value15;
		else if( name == "distance_Regulator_output")
				value = sample.value16;

		else if( name == "raw_encoder_Right")
			value = sample.value17;
		else if( name == "raw_encoder_Left")
			value = sample.value18;

		else if( name == "odo_X")
			value = sample.value19;
		else if( name == "odo_Y")
			value = sample.value20;
		else if( name == "odo_theta")
			value = sample.value21;

		else if( name == "CommandManager_X")
				value = sample.value22;
		else if( name == "CommandManager_Y")
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
