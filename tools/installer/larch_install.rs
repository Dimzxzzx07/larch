use std::process::Command;

struct Installer {
    disk: String,
    hostname: String,
    username: String,
    password: String,
}

impl Installer {
    fn new() -> Self {
        Installer {
            disk: String::from("/dev/sda"),
            hostname: String::from("larchbox"),
            username: String::from("user"),
            password: String::from("larch"),
        }
    }
    
    fn partition_disk(&self) {
        Command::new("parted")
            .args(&[&self.disk, "mklabel", "gpt"])
            .status();
        Command::new("parted")
            .args(&[&self.disk, "mkpart", "primary", "fat32", "1MiB", "512MiB"])
            .status();
        Command::new("parted")
            .args(&[&self.disk, "set", "1", "esp", "on"])
            .status();
        Command::new("parted")
            .args(&[&self.disk, "mkpart", "primary", "ext4", "512MiB", "100%"])
            .status();
    }
    
    fn format_filesystems(&self) {
        Command::new("mkfs.fat").args(&["-F32", "/dev/sda1"]).status();
        Command::new("mkfs.ext4").arg("/dev/sda2").status();
    }
    
    fn install_base(&self) {
        Command::new("mount").args(&["/dev/sda2", "/mnt"]).status();
        Command::new("mkdir").arg("-p").arg("/mnt/boot").status();
        Command::new("mount").args(&["/dev/sda1", "/mnt/boot"]).status();
        Command::new("cp").args(&["-r", "/larch_root/*", "/mnt/"]).status();
        Command::new("genfstab").args(&["-U", "/mnt", ">>", "/mnt/etc/fstab"]).status();
    }
    
    fn chroot_and_configure(&self) {
        Command::new("arch-chroot").arg("/mnt").args(&[
            "/bin/bash", "-c",
            &format!("echo '{}' > /etc/hostname", self.hostname)
        ]).status();
        Command::new("arch-chroot").arg("/mnt").args(&[
            "/bin/bash", "-c",
            &format!("echo '{}:{}' | chpasswd", self.username, self.password)
        ]).status();
    }
    
    fn run(&self) {
        self.partition_disk();
        self.format_filesystems();
        self.install_base();
        self.chroot_and_configure();
    }
}

fn main() {
    let installer = Installer::new();
    installer.run();
    println!("Larch installation complete!");
}