#include "AsservStreamControlPanel.h"

#include <unistd.h>  /* UNIX Standard Definitions      */

AsservStreamControlPanel::AsservStreamControlPanel(Ui_AsservStreamControlPanel *ui, AsservStream_uartDecoder *uartDecoder, int uartFd, int logFd ):
QMainWindow(),ui_(ui), uartFd_(uartFd), logFd_(logFd), uartDecoder(uartDecoder)
{
	ui->setupUi(this);
}


void AsservStreamControlPanel::send(char *buffer, size_t length)
{
	printf("Sending: \"%s\" \n",buffer);

	ssize_t size = write(uartFd_, buffer, length);
	if(size != length)
		printf("Error: Unable to send command\n");

	if( logFd_ != -1 )
	{
	    size = write(logFd_, buffer, length);
	    (void) size;
	    size = write(logFd_, "\n", 1);
	    (void) size;
	}
}

void AsservStreamControlPanel::on_reset_btn_clicked()
{
    char buffer[] = "asserv reset";
    int len = sizeof(buffer);
    send(buffer, len);
}

void AsservStreamControlPanel::on_motor_enable_btn_clicked()
{
    char buffer[] = "asserv enablemotor 1";
    int len = sizeof(buffer);
    send(buffer, len);
}

void AsservStreamControlPanel::on_motor_disable_btn_clicked()
{
    char buffer[] = "asserv enablemotor 0";
    int len = sizeof(buffer);
    send(buffer, len);
}

void AsservStreamControlPanel::on_distAngle_enable_btn_clicked()
{
    char buffer[] = "asserv enablepolar 1";
    int len = sizeof(buffer);
    send(buffer, len);
}

void AsservStreamControlPanel::on_distAngle_disable_btn_clicked()
{
    char buffer[] = "asserv enablepolar 0";
    int len = sizeof(buffer);
    send(buffer, len);
}

void AsservStreamControlPanel::on_vitesse_gauche_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv speedcontrol l %s %s",
                      ui_->vitesse_gauche_Kp->text().toStdString().c_str(),
                      ui_->vitesse_gauche_Ki->text().toStdString().c_str() );
    send(buffer, len);
}

void AsservStreamControlPanel::on_vitesse_droite_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv speedcontrol r %s %s",
                      ui_->vitesse_droite_Kp->text().toStdString().c_str(),
                      ui_->vitesse_droite_Ki->text().toStdString().c_str() );
    send(buffer, len);
}

void AsservStreamControlPanel::on_distance_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv distcontrol %s",
                      ui_->distance_Kp->text().toStdString().c_str() );
    send(buffer, len);
}
void AsservStreamControlPanel::on_angle_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv anglecontrol %s",
                      ui_->angle_Kp->text().toStdString().c_str() );
    send(buffer, len);
}

void AsservStreamControlPanel::on_angle_acc_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv angleacc %s",
              ui_->angle_acc->text().toStdString().c_str() );
    send(buffer, len);
}

void AsservStreamControlPanel::on_distance_acc_update_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv distacc %s %s %s",
              ui_->dist_acc_max->text().toStdString().c_str(),
              ui_->dist_acc_min->text().toStdString().c_str(),
			  ui_->dist_acc_threshold->text().toStdString().c_str()
			  );
    send(buffer, len);
}


void AsservStreamControlPanel::on_robot_speed_lin_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv robotfwspeedstep %s %s", ui_->robot_speed_lin_cmd->text().toStdString().c_str(),
            ui_->speed_consign_duration->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_robot_speed_ang_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv robotangspeedstep %s %s", ui_->robot_speed_ang_cmd->text().toStdString().c_str(),
            ui_->speed_consign_duration->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_left_whell_speed_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv wheelspeedstep l %s %s", ui_->left_wheel_speed_cmd->text().toStdString().c_str(),
            ui_->speed_consign_duration->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_right_whell_speed_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv wheelspeedstep r %s %s", ui_->right_wheel_speed_cmd->text().toStdString().c_str(),
            ui_->speed_consign_duration->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_pos_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv adddist %s", ui_->pos_cmd->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_angle_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv addangle %s", ui_->angle_cmd->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_goto_cmd_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv addgoto %s %s", ui_->goto_X->text().toStdString().c_str(), ui_->goto_Y->text().toStdString().c_str());
    send(buffer, len);
}

void AsservStreamControlPanel::on_get_config_btn_clicked()
{
    char buffer[128];
    int len = sprintf(buffer, "asserv get_config");
    send(buffer, len);
}

void AsservStreamControlPanel::on_update_config_btn_clicked()
{
	float *tab = (float*)uartDecoder->configBuffer;
	ui_->vitesse_gauche_Kp->setValue(tab[0]);
	ui_->vitesse_gauche_Ki->setValue(tab[1]);
	ui_->vitesse_droite_Kp->setValue(tab[2]);
	ui_->vitesse_droite_Ki->setValue(tab[3]);

}
