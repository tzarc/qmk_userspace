# @tzarc's QMK Userspace

This is @tzarc's variant of `qmk/qmk_userspace`.

### Testing QMK bootstrapper script

```sh
# !!!WARN!!! This has to be inside the context of a root user in a container, do not do this on your own system
useradd -m -s /bin/bash -d /home/testuser -U testuser

# Debian-like
apt update
apt install curl sudo -y

# RHEL-like
dnf install -y curl sudo

# Others do whatever your package manager does for the same packages

echo 'testuser:testpassword' | chpasswd || true
echo "root ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers  # some distros complain about root not being in sudoers
echo "testuser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
sudo -u testuser whoami || echo "Sudo test failed, but continuing..."
sudo -u testuser --preserve-env=GITHUB_TOKEN bash -li
```
