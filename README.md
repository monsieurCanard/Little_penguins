# Little_penguins
Series of challenges inspired by Eudyptula.

## Asignement 00
Pre-built version

Download latest Linus's Git tree on 
```
git.kernel.org
```

[!WARNING]
>You may choose whatever kernel configuration you pre-fer, but you must enable CONFIG_LOCALVERSION_AUTO=y.
> Pour verifier ca, il suffit d'ouvrir le fichier .config situe
> ```bash
> /usr/src/kernel-<kernel version>/.config
>```

### Procedure:
[!NOTE]
>Aller sur le site
>Tirer par date d'ajout 
>Chercher linus (ctr+f)
>Ouvrir le lien 
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/
>Prendre le dernier tar publie (actuellement linux-6.19-rc8.tar.gz)

### Installation



[!NOTE]
> On doit rendre le fichier kernel boot log file ainsi que le .config qui se trouver repectivement dans
```bash
/var/log/boot.log
```
et
```bash
/etc/
```

## Asignement 01

Create a "Hello World" kernel module with this following behaviour:
```bash
% sudo insmod main.ko
% dmesg | tail -1
[Wed May 13 12:59:18 2015] Hello world!
% sudo rmmod main.ko
% dmesg | tail -1
[Wed May 13 12:59:24 2015] Cleaning up module.
%
```

Explication:

**insmod** charge un module dans le noyau en cours d'utilisation.

**ko file** .ko is the extesion of a module file and .o is the extension of an object file.
An object file is created from compiling source files, with .c and .h extensions.
**dmesg** Afficher les messages liés au noyau sur les systèmes UNIX
**rmmod** Decharge de la memoire les modules charges dans le noyau en cour

### Comment faire un linux kernel module

Exemple de fichier c:
```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

// Module metadata
MODULE_AUTHOR("Ruan de Bruyn");
MODULE_DESCRIPTION("Hello world driver");
MODULE_LICENSE("GPL");

// Custom init and exit methods
static int __init custom_init(void) {
 printk(KERN_INFO "Hello world driver loaded.");
 return 0;
}

static void __exit custom_exit(void) {
 printk(KERN_INFO "Goodbye my friend, I shall miss you dearly...");
}

module_init(custom_init);
module_exit(custom_exit);
```

Si on decompose 

On commence par la creation des metadatas pour le module (mais pas obligatoire)

Ensuite on a des custom init et exit fonctions qui utilise printk(KERN INFO "Msg") ou KERN_INFO est le niveau de log

Et a la fin la liaison avec le noyau sinon le noyau ne sait pas quoi executer. 
[!NOTE]
>Ces macros disent au noyau :
>```c
>module_init(custom_init);
>```
>« Voici la fonction à appeler quand on charge le module »
>```c
>module_init(custom_exit);
>```
>« Voici celle à appeler quand on le retire »


Le sujet precise: 
[!WARNING]
>The module must compile on any system

Du coup ma premier idee est de mettre dans la regle compilation directement le header du kernel pour qu'il puisse etre recompiler pour chaque kernel et donc deviens standard

Makefile minimal pour un module kernel:
```Makefile
obj-m += mymodule.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

🔹 obj-m += mymodule.o

Indique au système de build du kernel que l’on souhaite compiler un module externe nommé mymodule.

mymodule.o est généré à partir de mymodule.c

Le résultat final sera : mymodule.ko

obj-m signifie module (par opposition à obj-y pour du code intégré au kernel)

🔹 KDIR := /lib/modules/$(shell uname -r)/build

Définit le chemin vers les headers et Makefiles du kernel en cours d’exécution.

uname -r retourne la version du kernel actif

/lib/modules/<version>/build est un lien vers les sources/headers du kernel

Garantit que le module est compilé pour le bon kernel

🔹 PWD := $(shell pwd)

Stocke le répertoire courant (là où se trouve le module).

Utilisé pour indiquer au kernel où se trouve le code du module

Nécessaire pour les modules externes

🔹 Cible all
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules


Demande au système de build du kernel de compiler le module.

-C $(KDIR) : se place dans le répertoire du kernel

M=$(PWD) : indique où se trouve le module externe

modules : cible kernel pour compiler des modules externes

👉 Le kernel :

choisit le bon compilateur

applique les bons flags

génère un .ko valide

🔹 Cible clean
```makefile
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

Nettoie les fichiers générés :
.o
.mod.c
.symvers
.ko


[!NOTE]
> Pour coller au mieux avec le sujet il faut utiliser la commande dmesg mais avec le -T pour mettre le timestamp en forme.


## Assignement 02 

Create patch for the Kernel git tree Makefile

Change EXTRAVERSION to includes "-thor_kernel" in its version string.

[!NOTE]
> Pour creer un fichier de patch il suffit de creer deux versions d'un meme fichier et faire la commande diff pour qu'on puisse utiliser la commande patch


## Assignement 03
Faut formatter un fichier c pour qu'il respecte le coding style du kernel linux

## Assignement 04

Il faut gerer l'usb d'un clavier.
Du coup on lit la bible sur ce sujet a cette adresse 
[USB Documentation](https://static.lwn.net/images/pdf/LDD3/ch13.pdf)
On comprend donc qu'il nous faut un module device dirver usb pour intervenir lors du hotplug d'un clavier usb et faire un printk pour afficher un message.

Structure usb_driver :
```c
static struct usb_driver skel_driver = {
        .name        = "skeleton",
        .probe       = skel_probe,
        .disconnect  = skel_disconnect,
        .suspend     = skel_suspend,
        .resume      = skel_resume,
        .pre_reset   = skel_pre_reset,
        .post_reset  = skel_post_reset,
        .id_table    = skel_table,
        .supports_autosuspend = 1,
};
```

Voila comment declarer un driver usb:
```c
static const struct usb_device_id usb_table[] = {
	{
		USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT, USB_INTERFACE_PROTOCOL_KEYBOARD)
	},
	{}
};


static int usb_keyboard_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
		printk(KERN_INFO "Keyboard connected, Hello World Keyboard !");
		return 0;
}

static void usb_keyboard_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "Keyboard removed or disapear in the void ! ");
}

static struct usb_driver usb_driver = {
	.name = "usb_keyboard_driver",
	.id_table = usb_table,
	.probe = usb_keyboard_probe,
	.disconnect = usb_keyboard_disconnect,
};
```

Et ensuite declarer les fonctions register et unregister du driver (comme dans l'assignement01) pour que le noyau puisse les appeler lors du chargement et du dechargement du module
[!NOTE]
>Pour register notre driver usb on utilise la fonction usb_register(&usb_driver) et pour le decharger on utilise usb_deregister(&usb_driver)

```c

static int __init custom_init(void)
{
	usb_register(&usb_keyboard_driver);
	return 0;
}

static void __exit custom_exit(void)
{
	usb_deregister(&usb_keyboard_driver);
}
```

[!NOTE]
>A faire pour expliquer ma phase de test et mes problemes sur mon clavier usb qui est directement pris en charge par les drivers generiques du kernel et qui ne declenche pas de hotplug. 


## Assignement 05

On va devoir develloper notre propre misc (miscellaneous) driver pour faire du character device et faire du read/write.
Pour faire c'est assez simple, contrairement l'assignement04, on va pas declarer un driver_usb mais un misc_device. Voila la structure d'un misc_device:
```c
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};
```

Il faut zoomer un peu sur la structure file operations car c'est grace a cette structure que l'on va pouvoir faire du read/write sur notre device, en utilisant la structure pour declarer nos propres fonctions de read/write. Voila la structure file_operations: 
```c
 struct file_operations {
       struct module *owner;
       loff_t (*llseek) (struct file *, loff_t, int);
       ssize_t (*read) (struct file *, char *, size_t, loff_t *);
       ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
       int (*readdir) (struct file *, void *, filldir_t);
       unsigned int (*poll) (struct file *, struct poll_table_struct *);
       int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
       int (*mmap) (struct file *, struct vm_area_struct *);
       int (*open) (struct inode *, struct file *);
       int (*flush) (struct file *);
       int (*release) (struct inode *, struct file *);
       int (*fsync) (struct file *, struct dentry *, int datasync);
       int (*fasync) (int, struct file *, int);
       int (*lock) (struct file *, int, struct file_lock *);
    	 ssize_t (*readv) (struct file *, const struct iovec *, unsigned long,
          loff_t *);
    	 ssize_t (*writev) (struct file *, const struct iovec *, unsigned long,
          loff_t *);
    };
```

Maintenant qu'on a nos structures, il nous reste plus qu'a coder notre my_read et my_write pour faire du read/write sur notre device. 
Voila comment on les declare dans la structure et dans le code:
```c
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos);

static ssize_t my_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos);
```
[!NOTE]
> Le pointeur de file est un pointeur vers une structure qui represente le fichier ouvert par l'utilisateur, il contient des informations sur le fichier et son etat.
> user_buf est un pointeur vers un buffer dans l'espace utilisateur ou on va ecrire ou lire les données.
> count est la taille des données a lire ou ecrire.
> ppos est un pointeur vers la position actuelle dans le fichier, il est utilisé pour savoir ou on en est dans le fichier pour les prochaines lectures ou ecritures.

[!WARNING]
>Dans cea fonctions on ne va pas utiliser les fonctions read and write mais copy_to_user et copy_from_user pour faire le lien entre l'espace kernel et l'espace utilisateur, car on ne peut pas acceder directement a l'espace utilisateur depuis le kernel.
> Il est donc tres important de faire attention a bien copier les données entre les deux espaces pour eviter les bugs et les failles de securite.

Exemple:
```c
static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[100];
	if (count > 100) {
		return -EINVAL;
	}
	if (copy_from_user(buf, user_buf, count)) {
		return -EFAULT;
	}
	printk(KERN_INFO "Received from user: %s\n", buf);
	return count;
}

static ssize_t my_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[] = "Hello from kernel!";
	size_t len = sizeof(buf);
	if (*ppos >= len) {
		return 0; // EOF
	}
	if (count > len - *ppos) {
		count = len - *ppos;
	}
	if (copy_to_user(user_buf, buf + *ppos, count)) {
		return -EFAULT;
	}
	//Ici on peut faire des operations sur le buffer avant de le copier a l'utilisateur, comme par exemple ajouter des informations sur le systeme ou des données dynamiques.
	*ppos += count;
	return count;
}
```

Apres avoir coder nos fonctions de read/write, il nous reste plus qu'a declarer notre misc_device et faire le lien avec le noyau pour que notre module puisse etre charge et decharge correctement.
```c
static struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "my_misc_device",
	.fops = &my_fops, // On lie notre structure de file_operations a notre misc_device
};

static int __init custom_init(void)
{
	int ret = misc_register(&my_misc_device);
	if (ret) {
		printk(KERN_ERR "Unable to register misc device\n");
		return ret;
	}
	printk(KERN_INFO "Misc device registered with minor %d\n", my_misc_device.minor);
	return 0;
}

static void __exit custom_exit(void)
{
	misc_deregister(&my_misc_device);
	printk(KERN_INFO "Misc device unregistered\n");
}
```

[!NOTE]
> MISC_DYNAMIC_MINOR permet au kernel de choisir automatiquement un numéro de minor disponible pour notre device (demande par le sujet)


Maintenant que notre module est codé, il nous reste plus qu'a le compiler et le tester pour voir si tout fonctionne correctement.

```bash
make
sudo insmod my_misc_device.ko
```

Verification que le module est active et que le device est cree grace a la commande dmesg et ls /dev/my_misc_device
```bash
dmesg | tail -1
[Wed May 13 13:00:00 2015] Misc device registered with minor 123

OU

ls /dev/my_misc_device
/dev/my_misc_device
```

Pour l'utiliser:

```bash
echo "Hello kernel" > /dev/my_misc_device
```
[!NOTE]
>Cette commande va ecrire "Hello kernel" dans notre device, ce qui va appeler la fonction my_write dans notre module.

```bash
cat /dev/my_misc_device
```
[!NOTE]
>Cette commande va lire les données de notre device, ce qui va appeler la fonction my_read dans notre module et afficher "Hello from kernel!" ou n'importe quoi d'autre dans le terminal.

[!WARNING]
> J'ai decouvert apres une grosse boucle infini sur mon write qu'il faut toujours return le nombre de bytes ecrit ou lu dans les fonctions de read/write pour que le kernel puisse savoir que l'operation s'est bien deroulee, sinon il va considerer que l'operation a echouee et retourner une erreur a l'utilisateur.
> Et pour le read il faut bien pense a deplacer le pointeur ppos pour que les prochaines lectures continuent a lire a partir de la position actuelle dans le fichier, sinon on va se retrouver avec une boucle infinie de lecture du meme buffer.

## Assignement 06

On doit teclecharger et installer le kernel linux-next.

[!NOTE]
> Linux-next est une branche de développement du kernel linux qui contient les dernières modifications et les nouvelles fonctionnalités qui seront intégrées dans les prochaines versions du kernel. C'est une branche de test pour les développeurs et les utilisateurs avancés qui souhaitent tester les dernières fonctionnalités du kernel avant leur intégration dans la branche principale.

On recupere le lien .tar de https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/
On telecharge le tar et on le decompresse dans /sources/
A l'interieur du dossier decompresse on enchaine les commandes de compilation et d'installation du kernel:
```bash
make mrproper
make defconfig
make -j$(nproc)
make modules_install
```
[!NOTE]
> make install ne fonctionne pas dans notre cas car nous n'avons pas LILO (Linux Loader) d'installé sur notre machine, du coup on va devoir faire le lien manuellement dans /boot et mettre a jour notre grub pour que le nouveau kernel soit pris en compte au demarrage.



```bash
cp -iv arch/x86/boot/bzImage /boot/vmlinuz-<version>
cp -iv System.map /boot/System.map-<version>
cp -iv .config /boot/config-<version>
```

Nous devons maintenant mettre a jour le fichier grub.cfg pour ajouter notre entree pour le nouveau kernel

Pour l'instant impossible de boot sur le dernier linux-next, j'ai peur que certaines modifications dans le kernel aient rendu mon systeme instable, du coup je vais devoir faire un rollback pour pouvoir continuer a travailler sur les autres assignements.


## Assignement 07
