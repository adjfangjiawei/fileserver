kubeadm reset
rm -r /etc/cni/net.d
ipvsadm --clear
systemctl stop containerd
rm -rf /var/lib/containerd
rm -rf /var/run/containerd