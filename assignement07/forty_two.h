#ifndef FORTY_TWO_H
#define FORTY_TWO_H

#include <linux/debugfs.h>

int id_register(struct dentry *dir);
void id_unregister(void);

#endif