#include <linux/types.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/list.h>

#include "unique.h"
#include "packet.h"
#include "cc2520.h"
#include "lpl.h"
#include "interface.h"
#include "debug.h"

struct node_list{
	struct list_head list;
	u64 src;
	u8 dsn;
};

int cc2520_unique_init(struct cc2520_dev *dev)
{
	INIT_LIST_HEAD(&dev->nodes);
	return 0;
}

void cc2520_unique_free(struct cc2520_dev *dev)
{
	struct node_list *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &dev->nodes) {
		tmp = list_entry(pos, struct node_list, list);
		list_del(pos);
		kfree(tmp);
	}
}

int cc2520_unique_tx(u8 * buf, u8 len, struct cc2520_dev *dev)
{
	return cc2520_lpl_tx(buf, len, dev);
}

void cc2520_unique_tx_done(u8 status, struct cc2520_dev *dev)
{
	cc2520_interface_tx_done(status, dev);
}
//////////////////
void cc2520_unique_rx_done(u8 *buf, u8 len, struct cc2520_dev *dev)
{
	struct node_list *tmp;
	u8 dsn;
	u64 src;
	bool found;
	bool drop;

	dsn = cc2520_packet_get_header(buf)->dsn;
	src = cc2520_packet_get_src(buf);

	found = false;
	drop = false;

	list_for_each_entry(tmp, &dev->nodes, list) {
		if (tmp->src == src) {
			found = true;
			if (tmp->dsn != dsn) {
				tmp->dsn = dsn;
			}
			else {
				drop = true;
			}
			break;
		}
	}

	if (!found) {
		tmp = kmalloc(sizeof(struct node_list), GFP_ATOMIC);
		if (tmp) {
			tmp->dsn = dsn;
			tmp->src = src;
			list_add(&(tmp->list), &dev->nodes);
			INFO(KERN_INFO, "unique%d found new mote: %lld\n", dev->id, src);
		}
		else {
			INFO(KERN_INFO, "unique%d alloc failed.\n", dev->id);
		}
	}

	if (!drop) {
		cc2520_interface_rx_done(buf, len, dev);
	}
}
