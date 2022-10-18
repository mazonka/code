#!/bin/sh

BLACK=$(tput setaf 0)
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
LIME_YELLOW=$(tput setaf 190)
POWDER_BLUE=$(tput setaf 153)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 7)
BRIGHT=$(tput bold)
NORMAL=$(tput sgr0)
BLINK=$(tput blink)
REVERSE=$(tput smso)
UNDERLINE=$(tput smul)

echo
echo "${LIME_YELLOW}${BRIGHT}Setting up VNC server${NORMAL}"

PROGRAM="./setup_vncserver.sh"
DEFAULT_GEOMETRY="1680x1050"
if [ "$#" -ge 1 ] && [ "$#" -le 2 ]; then
	IP=$1
	if expr "${IP}" : '[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$' >/dev/null; then
		:
	else
		echo "Invalid ip '${IP}'"
		exit
	fi
	if [ "$#" -eq 2 ]; then
		GEOMETRY=$2
	else
		echo "Using default display resolution '${DEFAULT_GEOMETRY}'"
		GEOMETRY=${DEFAULT_GEOMETRY}
	fi
	if expr "${GEOMETRY}" : '[0-9][0-9]*x[0-9][0-9]*$' >/dev/null; then
		:
	else
		echo "Invalid display resolution '${GEOMETRY}'"
		exit
	fi
else
	echo "USAGE: ${PROGRAM} IP [DISPLAY]"
	echo "DISPLAY: set the display resolution [DEFAULT: ${DEFAULT_GEOMETRY}]"
	exit
fi
echo "IP: ${IP} DISPLAY: ${GEOMETRY}"
echo

sudo apt update
sudo apt -y install openssh-server
sudo apt -y install vnc4server
sudo apt -y install xfce4 xfce4-goodies
sudo ufw allow 22

echo
echo "${LIME_YELLOW}${BRIGHT}Enter the VNC password${NORMAL}"
vncpasswd

VNC_HOME=${HOME}/.vnc
VNC_START0=${VNC_HOME}/start0.sh
VNC_STOP0=${VNC_HOME}/stop0.sh
AUTOSTART_DIR=${HOME}/.config/autostart

vncserver :1
vncserver -kill :1
echo "exec /usr/bin/startxfce4 &" >> ${VNC_HOME}/xstartup
#vncserver :1 -geometry 1280x800 -depth 24 -randr 1680x1050,1600x1200,1440x900,1280x800,1024x768,800x600
vncserver :1 -geometry ${GEOMETRY} -depth 24
#ssh ${USER}@${IP} -C -L ${PORT}:127.0.0.1:${PORT}
vncserver -kill :1

echo
echo "${LIME_YELLOW}${BRIGHT}Writing VNC services${NORMAL}"
sudo printf "[Unit]
Description=VNC server on secondary desktop
After=syslog.target network.target

[Service]
Type=forking
User=${USER}
Group=${USER}
WorkingDirectory=${HOME}

PIDFile=${VNC_HOME}/%%H:%%i.pid
ExecStartPre=-/usr/bin/vncserver -kill :%%i > /dev/null 2>&1
ExecStart=/usr/bin/vncserver -depth 24 -geometry ${GEOMETRY} :%%i
ExecStop=/usr/bin/vncserver -kill :%%i

[Install]
WantedBy=multi-user.target
" | sudo tee /etc/systemd/system/vncserver@.service

printf "/usr/bin/x0vncserver -display :0 -PasswordFile=${VNC_HOME}/passwd" | tee ${VNC_START0}

# printf "STATE=\`ps -a | grep x0vncserver | head -n1 | cut -d \" \" -f1\`
# if [ ! -z \"\$STATE\" ]; then
#	/bin/kill \${STATE}
# fi
# " | tee ${VNC_STOP0}
printf "/usr/bin/pkill v0vncserver" | tee ${VNC_STOP0}
chmod +x ${VNC_START0}
chmod +x ${VNC_STOP0}

if [ ! ${AUTOSTART_DIR} ]; then
	mkdir ${AUTOSTART_DIR}
fi

printf "[Desktop Entry]
Type=Application
Name=VNC server on primary desktop
Comment=VNC server on primary desktop
Path=${VNC_HOME}
Exec=${VNC_START0}
Terminal=false
Categories=Network
" | tee ${AUTOSTART_DIR}/x0vncserver.desktop

echo
echo "${LIME_YELLOW}${BRIGHT}Enabling VNC Server${NORMAL}"
sudo systemctl daemon-reload
sudo systemctl enable --now vncserver@1
# sudo systemctl enable --now vncserver0

echo
echo "${LIME_YELLOW}${BRIGHT}VNC server status (secondary)${NORMAL}"
echo "Press 'q' to continue"
systemctl status vncserver@1
printf "You can access this server on ${IP}:1"

echo
#echo "${LIME_YELLOW}${BRIGHT}VNC server status (primary)${NORMAL}"
#systemctl status vncserver0
echo "${LIME_YELLOW}${BRIGHT}VNC server status (primary)${NORMAL}"
echo "It is necessary to logout/login to activate the VNC server (primary)."
echo "You can access this server on ${IP}:0"

