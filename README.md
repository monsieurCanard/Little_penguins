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

Chaque usb liste dans le system fonctionne ainsi : 
```
root_hub-hub_port:config.interface
```


