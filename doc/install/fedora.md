Installing HYPERBORIA on Fedora
==========================
(last tested on Fedora 22, those with fedora versions older than 22 should substitute yum for dnf.)

## Install from dnf (easy)

```bash
sudo dnf install hyperboria
```

Skip straight down to "Generating a config"

## For development

### Prerequisites
```bash
sudo dnf install git nodejs gcc
```

### Getting hyperboria
```bash
git clone https://github.com/cjdelisle/hyperboria
cd hyperboria/
```

### Do you want to build crashey branch ?  (optional)

```bash
git checkout crashey
```

### Building hyperboria
```bash
./do
```

## Generating a config
```bash
./hyperboria-route --genconf | sudo tee /etc/hyperboria-route.conf
```

## Setting hyperboria to autostart on boot

```bash
sudo cp hyperboria.service /etc/systemd/system/hyperboria.service # This gives systemd some information about hyperboria.
sudo systemctl enable hyperboria.service #This sets hyperboria to be started on boot. if you don't want that, feel free to leave this line out.
sudo systemctl start hyperboria.service #This actually starts hyperboria.
```

## Check the logs
```bash
sudo systemctl status -l hyperboria
```
