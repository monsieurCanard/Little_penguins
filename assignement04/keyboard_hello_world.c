#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Script triggered when a keyboard is plugged in");
MODULE_LICENSE("GPL");


static const struct usb_device_id usb_table[] = {
	{
		USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_HID_SUBCLASS_BOOT, USB_INTERFACE_PROTOCOL_KEYBOARD)
	},
	{}
};
MODULE_DEVICE_TABLE(usb, usb_table);

static int usb_keyboard_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
		printk(KERN_INFO "Keyboard connected, Hello World Keyboard !");
		return 0;
}

static void usb_keyboard_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "Keyboard removed or disapear in the void ! ");
}


static struct usb_driver usb_keyboard_driver = {
	.name = "usb_keyboard_hello",
	.id_table = usb_table,
	.probe = usb_keyboard_probe,
	.disconnect = usb_keyboard_disconnect,
};

static int __init custom_init(void)
{
	usb_register(&usb_keyboard_driver);
	return 0;
}

static void __exit custom_exit(void)
{
	usb_deregister(&usb_keyboard_driver);
}

module_init(custom_init);
module_exit(custom_exit);

