#ifndef ASSERV_STREAM_CONTROL_PANEL_H
#define ASSERV_STREAM_CONTROL_PANEL_H

#include <QObject>
#include <QtPlugin>
#include <QMainWindow>
#include <map>
#include "PlotJuggler/statepublisher_base.h"
#include "ui_asservstreamcontrolpanel.h"


class AsservStreamControlPanel: public QMainWindow
{
    Q_OBJECT
public:
	explicit AsservStreamControlPanel(Ui_AsservStreamControlPanel *ui, int fd);
    ~ AsservStreamControlPanel(){}

    void closeEvent ( QCloseEvent* ) override{
        emit closed();
    }

public slots:
    void on_motor_enable_btn_clicked();
    void on_motor_disable_btn_clicked();
    void on_distAngle_enable_btn_clicked();
    void on_distAngle_disable_btn_clicked();
    void on_vitesse_gauche_update_btn_clicked();
    void on_vitesse_droite_update_btn_clicked();
    void on_distance_update_btn_clicked();
    void on_angle_update_btn_clicked();

    void on_robot_speed_lin_cmd_btn_clicked();
    void on_robot_speed_ang_cmd_btn_clicked();
    void on_left_whell_speed_cmd_btn_clicked();
    void on_right_whell_speed_cmd_btn_clicked();

    void on_pos_cmd_btn_clicked();
    void on_angle_cmd_btn_clicked();
    void on_goto_cmd_btn_clicked();

signals:
    void closed();

private:

    void send(char *buffer, size_t length);
    Ui_AsservStreamControlPanel *ui_;
    int fd_;
};



#endif // ASSERV_STREAM_CONTROL_PANEL_H
