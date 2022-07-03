#include <allegro.h>
#include <a5alleg.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include "native.h"

static ALLEGRO_MENU * native_menu[EOF_MAX_NATIVE_MENUS] = {NULL};
static int native_menu_items[EOF_MAX_NATIVE_MENUS] = {0};
static MENU * a4_menu[EOF_MAX_NATIVE_MENUS] = {NULL};
static MENU * a4_menu_item[EOF_MAX_MENU_ITEMS] = {NULL};
static char * a4_menu_item_name[EOF_MAX_MENU_ITEMS] = {NULL};
static int a4_menu_item_flags[EOF_MAX_MENU_ITEMS] = {0};
static bool native_menu_blank[EOF_MAX_MENU_ITEMS] = {false};
static int current_menu = 0;
static int current_id = 0;
static ALLEGRO_EVENT_QUEUE * event_queue = NULL;

static const char * get_menu_text(const char * in, char * out)
{
	int i;
	int c = 0;

	if(!in || !strlen(in))
	{
		return NULL;
	}
	for(i = 0; i < strlen(in); i++)
	{
		if(in[i] == '\t')
		{
			break;
		}
		else if(in[i] != '&')
		{
			out[c] = in[i];
			c++;
			out[c] = 0;
		}
	}
	return out;
}

static bool index_a4_menu_item(int id, MENU * mp)
{
	a4_menu_item[id] = mp;
	a4_menu_item_name[id] = mp->text ? malloc(strlen(mp->text) + 1) : NULL;
	if(a4_menu_item_name[id])
	{
		strcpy(a4_menu_item_name[id], mp->text);
	}
	a4_menu_item_flags[id] = mp->flags;
	return true;
}

static bool update_a4_menu_item(int id, const char * caption, int flags)
{
	if(a4_menu_item_name[id])
	{
		free(a4_menu_item_name[id]);
	}
	a4_menu_item_name[id] = caption ? malloc(strlen(caption) + 1) : NULL;
	if(a4_menu_item_name[id])
	{
		strcpy(a4_menu_item_name[id], caption);
	}
	a4_menu_item_flags[id] = flags;
	return true;
}

static bool add_menu(MENU * mp, MENU * parent, ALLEGRO_MENU * native_parent)
{
	int this_menu = current_menu;
	char buf[256];
	int flags;

	if(this_menu >= EOF_MAX_NATIVE_MENUS)
	{
		return false;
	}
	native_menu[this_menu] = al_create_menu();
	if(!native_menu[this_menu])
	{
		return false;
	}
	a4_menu[this_menu] = mp;
	current_menu++;
	printf("walk menu\n");
	while(mp && mp->text)
	{
		/* Allegro 4 menu items are also menus if they have a child. Add new native
	     menu if we have a child. */
		printf("%s\n", mp->text);
		if(mp->child)
		{
			if(!add_menu(mp->child, mp, native_menu[this_menu]))
			{
				return false;
			}
		}

		/* Add menu item to current menu. */
		else
		{
			flags = 0;
			if(mp->flags & D_SELECTED)
			{
				flags = ALLEGRO_MENU_ITEM_CHECKED;
			}
			if(mp->flags & D_DISABLED)
			{
				flags = ALLEGRO_MENU_ITEM_DISABLED;
			}
			if(mp->text)
			{
				al_append_menu_item(native_menu[this_menu], get_menu_text(mp->text, buf), current_id, flags, NULL, NULL);
				native_menu_items[this_menu]++;
				index_a4_menu_item(current_id, mp);
				current_id++;
			}
		}
		mp++;
	}

	printf("whatever happens next\n");
	if(parent && native_parent)
	{
		flags = 0;
		if(mp->flags & D_SELECTED)
		{
			flags = ALLEGRO_MENU_ITEM_CHECKED;
		}
		if(mp->flags & D_DISABLED)
		{
			flags = ALLEGRO_MENU_ITEM_DISABLED;
		}
//		if(mp->text)
		{
			printf("%s\n", mp->text);
			al_append_menu_item(native_parent, get_menu_text(parent->text, buf), current_id, flags, NULL, native_menu[this_menu]);
			index_a4_menu_item(current_id, mp);
			current_id++;
		}
	}
	printf("done\n");
	return true;
}

static void destroy_native_menus(void)
{
	int i;

	for(i = current_menu - 1; i >= 0; i--)
	{
		al_destroy_menu(native_menu[i]);
	}
	current_menu = 0;
}

bool eof_set_up_native_menus(MENU * mp)
{
	int i = 0;
	int pos = 0;

	event_queue = al_create_event_queue();
	if(!event_queue)
	{
		return false;
	}
	if(!add_menu(mp, NULL, NULL))
	{
		destroy_native_menus();
		return false;
	}
	al_register_event_source(event_queue, al_get_default_menu_event_source());
	al_set_display_menu(all_get_display(), native_menu[0]);
	return true;
}

static bool set_menu_item_flags(ALLEGRO_MENU * mp, int item, int flags)
{
  int old_flags = al_get_menu_item_flags(mp, item) & ~ALLEGRO_MENU_ITEM_CHECKBOX;

	if(old_flags == -1)
	{
		return false;
	}
  if(flags != old_flags)
  {
    al_set_menu_item_flags(mp, item, flags);
  }
	return true;
}

static bool flags_changed(int mflags, int nmflags)
{
	int cflags = 0;

	if(nmflags & ALLEGRO_MENU_ITEM_DISABLED)
	{
		if(!(mflags & D_DISABLED))
		{
			return true;
		}
	}
	if(!(nmflags & ALLEGRO_MENU_ITEM_DISABLED))
	{
		if(mflags & D_DISABLED)
		{
			return true;
		}
	}
	if(nmflags & ALLEGRO_MENU_ITEM_CHECKED)
	{
		if(!(mflags & D_SELECTED))
		{
			return true;
		}
	}
	if(!(nmflags & ALLEGRO_MENU_ITEM_CHECKED))
	{
		if(mflags & D_SELECTED)
		{
			return true;
		}
	}
	return false;
}

static bool caption_changed(const char * mcap, const char * nmcap)
{
	if(mcap && !nmcap)
	{
		return true;
	}
	else if(nmcap && !mcap)
	{
		return true;
	}
	else if(mcap && nmcap && strcmp(mcap, nmcap))
	{
		return true;
	}
	return false;
}

static bool update_native_menu(int m)
{
	ALLEGRO_MENU * mp = NULL;
	int index = -1;
	int i;
	bool update = false;
	int new_flags = 0;

	for(i = 0; i < current_menu; i++)
	{
		if(al_find_menu_item(native_menu[i], m, &mp, &index))
		{
			if(caption_changed(a4_menu_item_name[m], a4_menu_item[m]->text))
			{
				printf("update %s\n", a4_menu_item[m]->text);
				al_set_menu_item_caption(mp, index, a4_menu_item[m]->text ? a4_menu_item[m]->text : "");
				update = true;
				printf("update done\n");
			}
			if(a4_menu_item_flags[m] != a4_menu_item[m]->flags)
			{
				if(a4_menu_item[m]->flags & D_DISABLED)
				{
					new_flags = ALLEGRO_MENU_ITEM_DISABLED;
				}
				if(a4_menu_item[m]->flags & D_SELECTED)
				{
					new_flags = ALLEGRO_MENU_ITEM_CHECKED;
				}
				set_menu_item_flags(mp, index, new_flags);
			}
			if(update)
			{
				update_a4_menu_item(m, a4_menu_item[i]->text, a4_menu_item[i]->flags);
			}
			return true;
		}
	}
	return false;
}

void eof_update_native_menus(void)
{
	int i;

	for(i = 0; i < current_id; i++)
	{
		update_native_menu(i);
	}
}

static void call_menu_proc(int id)
{
	printf("call 1\n");
	if(a4_menu_item[id]->proc)
	{
	printf("call 2\n");
		a4_menu_item[id]->proc();
	printf("call 3\n");
	}
}

void eof_handle_native_menu_clicks(void)
{
	ALLEGRO_EVENT event;

	while(!al_event_queue_is_empty(event_queue))
	{
		al_get_next_event(event_queue, &event);
		if(event.type == ALLEGRO_EVENT_MENU_CLICK)
		{
			call_menu_proc(event.user.data1);
		}
	}
}
