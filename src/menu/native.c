#include <allegro.h>
#include <a5alleg.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include "native.h"

static ALLEGRO_MENU * native_menu[EOF_MAX_NATIVE_MENUS] = {NULL};
static MENU * a4_menu[EOF_MAX_NATIVE_MENUS] = {NULL};
static int current_menu = 0;

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
	while(mp && mp->text)
	{
		if(mp->child)
		{
			if(!add_menu(mp->child, mp, native_menu[this_menu]))
			{
				return false;
			}
		}
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
				al_append_menu_item(native_menu[this_menu], get_menu_text(mp->text, buf), this_menu, flags, NULL, NULL);
			}
		}
		mp++;
	}
	if(parent && native_parent)
	{
		al_append_menu_item(native_parent, get_menu_text(parent->text, buf), this_menu, flags, NULL, native_menu[this_menu]);
	}
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

	if(!add_menu(mp, NULL, NULL))
	{
		destroy_native_menus();
		return false;
	}
	al_set_display_menu(all_get_display(), native_menu[0]);
	return true;
}

static void set_menu_item_flags(ALLEGRO_MENU * mp, int item, int flags)
{
  int old_flags = al_get_menu_item_flags(mp, item) & ~ALLEGRO_MENU_ITEM_CHECKBOX;

  if(flags != old_flags)
  {
    al_set_menu_item_flags(mp, item, flags);
  }
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

static void update_native_menu_flags(MENU * mp, ALLEGRO_MENU * nmp)
{
//	if(flags_changed(mp->flags, al_get_menu_item_flags()))
}



void eof_update_native_menus(MENU * mp)
{
	int i;

	for(i = 0; i < current_menu; i++)
	{
		update_native_menu_flags(a4_menu[i], native_menu[i]);
	}
}
