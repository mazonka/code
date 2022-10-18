pkill Xvnc4
sudo rm -f /etc/systemd/system/vncserver@.service
rm -rf ~/.ssh
rm -rf ~/.vnc
rm -f ~/.config/autostart/x0vncserver.desktop
sudo apt -y --purge remove openssh-server
sudo apt -y --purge remove vnc4server
sudo apt -y --purge remove xfce4 xfce4-goodies

