
touch /opt/IBM/informix/chunk1
chmod 660 /opt/IBM/informix/chunk1
chown informix:informix /opt/IBM/informix/chunk1

onspaces -c -S sbspace -p /opt/IBM/informix/chunk1 -o 0 -s 20000 -Df LOGGING=ON


ontape -s -L 0
