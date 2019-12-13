#include "AsservStreamControlPanel.h"

#include <unistd.h>  /* UNIX Standard Definitions      */

AsservStreamControlPanel::AsservStreamControlPanel(Ui_AsservStreamControlPanel *ui, int fd):
QMainWindow(),ui_(ui), fd_(fd)
{
	ui->setupUi(this);
}


void AsservStreamControlPanel::send(char *buffer, size_t length)
{
	printf("Send: \"%s\" \n",buffer);

	ssize_t size = write(fd_, buffer, length);
	if(size != length)
		printf("Error: Unable to send command\n");
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

