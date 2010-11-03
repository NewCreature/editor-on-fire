#include <allegro.h>
#include <math.h>	//For sqrt()
#include "../agup/agup.h"
#include "../main.h"
#include "../dialog.h"
#include "../mix.h"
#include "../beat.h"
#include "../utility.h"
#include "../midi.h"
#include "../ini.h"
#include "../dialog/proc.h"
#include "../undo.h"
#include "../player.h"
#include "../waveform.h"
#include "../silence.h"
#include "song.h"

char eof_track_selected_menu_text[6][32] = {" PART &GUITAR", " PART &BASS", " PART GUITAR &COOP", " PART &RHYTHM", " PART &DRUMS", " PART &VOCALS"};

MENU eof_song_seek_bookmark_menu[] =
{
    {"&0\tNum 0", eof_menu_song_seek_bookmark_0, NULL, 0, NULL},
    {"&1\tNum 1", eof_menu_song_seek_bookmark_1, NULL, 0, NULL},
    {"&2\tNum 2", eof_menu_song_seek_bookmark_2, NULL, 0, NULL},
    {"&3\tNum 3", eof_menu_song_seek_bookmark_3, NULL, 0, NULL},
    {"&4\tNum 4", eof_menu_song_seek_bookmark_4, NULL, 0, NULL},
    {"&5\tNum 5", eof_menu_song_seek_bookmark_5, NULL, 0, NULL},
    {"&6\tNum 6", eof_menu_song_seek_bookmark_6, NULL, 0, NULL},
    {"&7\tNum 7", eof_menu_song_seek_bookmark_7, NULL, 0, NULL},
    {"&8\tNum 8", eof_menu_song_seek_bookmark_8, NULL, 0, NULL},
    {"&9\tNum 9", eof_menu_song_seek_bookmark_9, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU eof_song_seek_menu[] =
{
    {"Start\tHome", eof_menu_song_seek_start, NULL, 0, NULL},
    {"End\tEnd", eof_menu_song_seek_end, NULL, 0, NULL},
    {"Rewind\tR", eof_menu_song_seek_rewind, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"First Note\tCtrl+Home", eof_menu_song_seek_first_note, NULL, 0, NULL},
    {"Last Note\tCtrl+End", eof_menu_song_seek_last_note, NULL, 0, NULL},
    {"Previous Note\tShift+PGUP", eof_menu_song_seek_previous_note, NULL, 0, NULL},
    {"Next Note\tShift+PGDN", eof_menu_song_seek_next_note, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"Previous Screen\tCtrl+PGUP", eof_menu_song_seek_previous_screen, NULL, 0, NULL},
    {"Next Screen\tCtrl+PGDN", eof_menu_song_seek_next_screen, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Bookmark", NULL, eof_song_seek_bookmark_menu, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU eof_track_selected_menu[] =
{
    {eof_track_selected_menu_text[0], eof_menu_track_selected_guitar, NULL, D_SELECTED, NULL},
    {eof_track_selected_menu_text[1], eof_menu_track_selected_bass, NULL, 0, NULL},
    {eof_track_selected_menu_text[2], eof_menu_track_selected_guitar_coop, NULL, 0, NULL},
    {eof_track_selected_menu_text[3], eof_menu_track_selected_rhythm, NULL, 0, NULL},
    {eof_track_selected_menu_text[4], eof_menu_track_selected_drum, NULL, 0, NULL},
    {eof_track_selected_menu_text[5], eof_menu_track_selected_vocals, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU eof_track_menu[] =
{
    {"&Selected", NULL, eof_track_selected_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Properties", NULL, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU eof_catalog_menu[] =
{
    {"&Show\tQ", eof_menu_catalog_show, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Add", eof_menu_catalog_add, NULL, 0, NULL},
    {"&Delete", eof_menu_catalog_delete, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Previous\tW", eof_menu_catalog_previous, NULL, 0, NULL},
    {"&Next\tE", eof_menu_catalog_next, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

MENU eof_waveform_menu[] =
{
	{"&Show\tF5", eof_menu_song_waveform, NULL, 0, NULL},
	{"&Configure", eof_menu_song_waveform_settings, NULL, 0, NULL},
	{NULL, NULL, NULL, 0, NULL}
};

MENU eof_song_menu[] =
{
    {"&Seek", NULL, eof_song_seek_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Track", NULL, eof_track_selected_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&Catalog", NULL, eof_catalog_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"&File Info", eof_menu_song_file_info, NULL, 0, NULL},
    {"&INI Settings", eof_menu_song_ini_settings, NULL, 0, NULL},
    {"&Properties\tF9", eof_menu_song_properties, NULL, 0, NULL},
	{"&Leading Silence", eof_menu_song_add_silence, NULL, 0, NULL},
    {"", NULL, NULL, 0, NULL},
	{"&Audio cues", eof_menu_audio_cues, NULL, 0, NULL},
	{"&Waveform Graph", NULL, eof_waveform_menu, 0, NULL},
    {"", NULL, NULL, 0, NULL},
    {"T&est In FOF\tF12", eof_menu_song_test, NULL, EOF_LINUX_DISABLE, NULL},
    {NULL, NULL, NULL, 0, NULL}
};

DIALOG eof_ini_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_agup_window_proc,    0,  48,  216 + 110 + 20, 160 + 72, 2,   23,  0,    0,      0,   0,   "INI Settings",               NULL, NULL },
   { d_agup_list_proc,   12, 84,  110 * 2 + 20,  69 * 2,  2,   23,  0,    0,      0,   0,   eof_ini_list, NULL, NULL },
   { d_agup_push_proc, 134 + 130,  84, 68,  28, 2,   23,  0,    D_EXIT, 0,   0,   "Add",               NULL, eof_ini_dialog_add },
   { d_agup_push_proc, 134 + 130,  124, 68,  28, 2,   23,  0,    D_EXIT, 0,   0,   "Delete",               NULL, eof_ini_dialog_delete },
   { d_agup_button_proc, 12,  166 + 69, 190 + 30 + 20,  28, 2,   23,  0,    D_EXIT, 0,   0,   "Done",               NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG eof_ini_add_dialog[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)           (dp2) (dp3) */
   { d_agup_window_proc,    0,  48,  204 + 110, 106, 2,   23,  0,    0,      0,   0,   "Add INI Setting",               NULL, NULL },
   { d_agup_text_proc,   12,  84,  64,  8,  2,   23,  0,    0,      0,   0,   "Text:",         NULL, NULL },
   { d_agup_edit_proc,   48, 80,  144 + 110,  20,  2,   23,  0,    0,      255,   0,   eof_etext,           NULL, NULL },
   { d_agup_button_proc, 12 + 55,  112, 84,  28, 2,   23,  '\r',    D_EXIT, 0,   0,   "OK",               NULL, NULL },
   { d_agup_button_proc, 108 + 55, 112, 78,  28, 2,   23,  0,    D_EXIT, 0,   0,   "Cancel",           NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG eof_song_properties_dialog[] =
{
   /* (proc)              (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                    (dp2) (dp3) */
   { d_agup_window_proc,  0,   0,   440, 240, 0,   0,   0,    0,      0,   0,   "Song Properties",      NULL, NULL },
   { d_agup_text_proc,    12,  40,  80,  12,  0,   0,   0,    0,      0,   0,   "Song Title",           NULL, NULL },
   { d_agup_edit_proc,    12,  56,  184, 20,  0,   0,   0,    0,      255, 0,   eof_etext,              NULL, NULL },
   { d_agup_text_proc,    12,  88,  96,  12,  0,   0,   0,    0,      0,   0,   "Artist",               NULL, NULL },
   { d_agup_edit_proc,    12,  104, 184, 20,  0,   0,   0,    0,      255, 0,   eof_etext2,             NULL, NULL },
   { d_agup_text_proc,    12,  136, 108, 12,  0,   0,   0,    0,      0,   0,   "Frettist",             NULL, NULL },
   { d_agup_edit_proc,    12,  152, 184, 20,  0,   0,   0,    0,      255, 0,   eof_etext3,             NULL, NULL },
   { d_agup_text_proc,    12,  184, 60,  12,  0,   0,   0,    0,      0,   0,   "Delay",                NULL, NULL },
   { eof_verified_edit_proc,    12,  204, 60,  20,  0,   0,   0,    0,      6,   0, eof_etext4, "0123456789", NULL },
   { d_agup_text_proc,    84,  184, 48,  12,  0,   0,   0,    0,      0,   0,   "Year",                 NULL, NULL },
   { eof_verified_edit_proc,    84,  204, 48,  20,  0,   0,   0,    0,      4,   0, eof_etext5, "0123456789", NULL },
   { d_agup_text_proc,    208, 40,  96,  12,  0,   0,   0,    0,      0,   0,   "Loading Text",         NULL, NULL },
   { d_agup_edit_proc,    208, 56,  216, 20,  0,   0,   0,    0,      255, 0,   eof_etext6,             NULL, NULL },
   { d_agup_text_proc,    208, 88,  96,  12,  0,   0,   0,    0,      0,   0,   "Loading Text Preview", NULL, NULL },
   { d_agup_textbox_proc, 208, 104, 216, 68,  0,   0,   0,    0,      0,   0,   eof_etext6,             NULL, NULL },
   { d_agup_check_proc,   160, 188, 136, 16,  0,   0,   0,    0,      1,   0,   "Lyrics",               NULL, NULL },
   { d_agup_check_proc,   160, 208, 136, 16,  0,   0,   0,    0,      1,   0,   "8th Note HO/PO",       NULL, NULL },
   { d_agup_button_proc,  340, 200, 84,  24,  0,   0,   '\r', D_EXIT, 0,   0,   "OK",                   NULL, NULL },
   { NULL,                0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                   NULL, NULL }
};

void eof_prepare_song_menu(void)
{
	int i;
	int firstnote = -1;
	int lastnote = -1;
	int noted[4] = {0};
	int seekp = 0;
	int seekn = 0;

	if(eof_song && eof_song_loaded)
	{
		if(eof_vocals_selected)
		{
			for(i = 0; i < eof_song->vocal_track->lyrics; i++)
			{
				if(firstnote < 0)
				{
					firstnote = i;
				}
				lastnote = i;
				if(eof_song->vocal_track->lyric[i]->pos < ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0))
				{
					seekp = 1;
				}
				if(eof_song->vocal_track->lyric[i]->pos > ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0))
				{
					seekn = 1;
				}
			}
		}
		else
		{
			for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
			{
				if(eof_song->track[eof_selected_track]->note[i]->type == eof_note_type)
				{
					if(firstnote < 0)
					{
						firstnote = i;
					}
					lastnote = i;
				}
				if((eof_song->track[eof_selected_track]->note[i]->type >= 0) && (eof_song->track[eof_selected_track]->note[i]->type < 4))
				{
					noted[(int)eof_song->track[eof_selected_track]->note[i]->type] = 1;
				}
				if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos < ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0)))
				{
					seekp = 1;
				}
				if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos > ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0)))
				{
					seekn = 1;
				}
			}
		}

		/* track */
		for(i = 0; i < EOF_MAX_TRACKS + 1; i++)
		{
			eof_track_selected_menu[i].flags = 0;
		}
		if(eof_vocals_selected)
		{
			eof_track_selected_menu[EOF_TRACK_VOCALS].flags = D_SELECTED;
		}
		else
		{
			eof_track_selected_menu[eof_selected_track].flags = D_SELECTED;
		}

		/* seek start */
		if(eof_music_pos == 0)
		{
			eof_song_seek_menu[0].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[0].flags = 0;
		}

		/* seek end */
		if(eof_music_pos >= eof_music_actual_length - 1)
		{
			eof_song_seek_menu[1].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[1].flags = 0;
		}

		/* rewind */
		if(eof_music_pos == eof_music_rewind_pos)
		{
			eof_song_seek_menu[2].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[2].flags = 0;
		}
		if(eof_vocals_selected)
		{
			if(eof_song->vocal_track->lyrics)
			{

				/* seek first note */
				if(eof_song->vocal_track->lyric[0]->pos == eof_music_pos - eof_av_delay)
				{
					eof_song_seek_menu[4].flags = D_DISABLED;
				}
				else
				{
					eof_song_seek_menu[4].flags = 0;
				}

				/* seek last note */
				if(eof_song->vocal_track->lyric[eof_song->vocal_track->lyrics - 1]->pos == eof_music_pos - eof_av_delay)
				{
					eof_song_seek_menu[5].flags = D_DISABLED;
				}
				else
				{
					eof_song_seek_menu[5].flags = 0;
				}
			}
			else
			{
				eof_song_seek_menu[4].flags = D_DISABLED; // seek first note
				eof_song_seek_menu[5].flags = D_DISABLED; // seek last note
			}
		}
		else
		{
			if(noted[eof_note_type])
			{

				/* seek first note */
				if((firstnote >= 0) && (eof_song->track[eof_selected_track]->note[firstnote]->pos == eof_music_pos - eof_av_delay))
				{
					eof_song_seek_menu[4].flags = D_DISABLED;
				}
				else
				{
					eof_song_seek_menu[4].flags = 0;
				}

				/* seek last note */
				if((lastnote >= 0) && (eof_song->track[eof_selected_track]->note[lastnote]->pos == eof_music_pos - eof_av_delay))
				{
					eof_song_seek_menu[5].flags = D_DISABLED;
				}
				else
				{
					eof_song_seek_menu[5].flags = 0;
				}
			}
			else
			{
				eof_song_seek_menu[4].flags = D_DISABLED; // seek first note
				eof_song_seek_menu[5].flags = D_DISABLED; // seek last note
			}
		}

		/* seek previous note */
		if(seekp)
		{
			eof_song_seek_menu[6].flags = 0;
		}
		else
		{
			eof_song_seek_menu[6].flags = D_DISABLED;
		}

		/* seek next note */
		if(seekn)
		{
			eof_song_seek_menu[7].flags = 0;
		}
		else
		{
			eof_song_seek_menu[7].flags = D_DISABLED;
		}

		/* seek next screen */
		if(eof_music_pos <= eof_av_delay)
		{
			eof_song_seek_menu[9].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[9].flags = 0;
		}

		/* seek previous screen */
		if(eof_music_pos >= eof_music_actual_length - 1)
		{
			eof_song_seek_menu[10].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[10].flags = 0;
		}

		/* seek bookmark # */
		char bmcount = 0;
		for(i = 0; i < EOF_MAX_BOOKMARK_ENTRIES; i++)
		{
			if(eof_song->bookmark_pos[i] != 0)
			{
				eof_song_seek_bookmark_menu[i].flags = 0;
				bmcount++;
			}
			else
			{
				eof_song_seek_bookmark_menu[i].flags = D_DISABLED;
			}
		}

		/* seek bookmark */
		if(bmcount == 0)
		{
			eof_song_seek_menu[12].flags = D_DISABLED;
		}
		else
		{
			eof_song_seek_menu[12].flags = 0;
		}

		/* show catalog */
		if(eof_song->catalog->entries > 0)
		{
			eof_catalog_menu[0].flags = eof_catalog_menu[0].flags & D_SELECTED;
		}
		else
		{
			eof_catalog_menu[0].flags = D_DISABLED;
		}

		/* add catalog entry */
		if(eof_count_selected_notes(NULL,0))	//If there are notes selected
		{
			eof_catalog_menu[2].flags = 0;
		}
		else
		{
			eof_catalog_menu[2].flags = D_DISABLED;
		}

		/* remove catalog entry */
		if(eof_selected_catalog_entry < eof_song->catalog->entries)
		{
			eof_catalog_menu[3].flags = 0;
		}
		else
		{
			eof_catalog_menu[3].flags = D_DISABLED;
		}

		/* previous/next catalog entry */
		if(eof_song->catalog->entries > 1)
		{
			eof_catalog_menu[5].flags = 0;
			eof_catalog_menu[6].flags = 0;
		}
		else
		{
			eof_catalog_menu[5].flags = D_DISABLED;
			eof_catalog_menu[6].flags = D_DISABLED;
		}

		/* catalog */
		if((eof_catalog_menu[0].flags & D_DISABLED) && (eof_catalog_menu[2].flags & D_DISABLED) && (eof_catalog_menu[3].flags & D_DISABLED) && (eof_catalog_menu[5].flags & D_DISABLED) && (eof_catalog_menu[6].flags & D_DISABLED))
		{
			eof_song_menu[4].flags = D_DISABLED;
		}
		else
		{
			eof_song_menu[4].flags = 0;
		}

		/* track */
		for(i = 0; i < EOF_MAX_TRACKS; i++)
		{
			if(eof_song->track[i]->notes > 0)
			{
				eof_track_selected_menu[i].text[0] = '*';
			}
			else
			{
				eof_track_selected_menu[i].text[0] = ' ';
			}
		}
		if(eof_song->vocal_track->lyrics > 0)
		{
			eof_track_selected_menu[EOF_TRACK_VOCALS].text[0] = '*';
		}
		else
		{
			eof_track_selected_menu[EOF_TRACK_VOCALS].text[0] = ' ';
		}
		if(eof_supports_silence)
		{
			eof_song_menu[9].flags = 0;
		}
		else
		{
			eof_song_menu[9].flags = D_DISABLED;
		}
	}
}

int eof_menu_song_seek_start(void)
{
	char wasplaying = 0;
	if(!eof_music_catalog_playback)
	{
		if(!eof_music_paused)	//If the chart is already playing
		{
			eof_music_play();	//stop it
			wasplaying = 1;
		}
		alogg_seek_abs_msecs_ogg(eof_music_track, 0);
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_music_pos = eof_av_delay;
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
		if(wasplaying)
		{	//If the playback was stopped to rewind the seek position
			eof_music_play();	//Resume playing
		}
	}
	return 1;
}

int eof_menu_song_seek_end(void)
{
	if(!eof_music_catalog_playback)
	{
		alogg_seek_abs_msecs_ogg(eof_music_track, eof_music_actual_length - 1);
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_music_pos = eof_music_actual_length - 1;
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
	}
	return 1;
}

int eof_menu_song_seek_rewind(void)
{
	if(!eof_music_catalog_playback)
	{
		alogg_seek_abs_msecs_ogg(eof_music_track, eof_music_rewind_pos);
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_music_pos = eof_music_rewind_pos;
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
	}
	return 1;
}

int eof_menu_song_seek_first_note_vocals(void)
{
	int i;
	int first_pos = -1;

	if(!eof_music_catalog_playback)
	{
		for(i = 0; i < eof_song->vocal_track->lyrics; i++)
		{
			if(eof_song->vocal_track->lyric[i]->pos < first_pos)
			{
				first_pos = eof_song->vocal_track->lyric[i]->pos;
			}
		}
		alogg_seek_abs_msecs_ogg(eof_music_track, first_pos + eof_av_delay);
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_music_pos = first_pos + eof_av_delay;
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
	}
	return 1;
}

int eof_menu_song_seek_first_note(void)
{
	int i;
	int first_pos = -1;

	if(eof_vocals_selected)
	{
		return eof_menu_song_seek_first_note_vocals();
	}
	else
	{
		if(!eof_music_catalog_playback)
		{
			for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
			{
				if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos < first_pos))
				{
					first_pos = eof_song->track[eof_selected_track]->note[i]->pos;
				}
			}
			alogg_seek_abs_msecs_ogg(eof_music_track, first_pos + eof_av_delay);
			eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_pos = first_pos + eof_av_delay;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
		}
	}
	return 1;
}

int eof_menu_song_seek_last_note_vocals(void)
{
	int i;
	unsigned long last_pos = 0;

	if(!eof_music_catalog_playback)
	{
		for(i = 0; i < eof_song->vocal_track->lyrics; i++)
		{
			if((eof_song->vocal_track->lyric[i]->pos > last_pos) && (eof_song->vocal_track->lyric[i]->pos < eof_music_length))
			{
				last_pos = eof_song->vocal_track->lyric[i]->pos;
			}
		}
		alogg_seek_abs_msecs_ogg(eof_music_track, last_pos + eof_av_delay);
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_music_pos = last_pos + eof_av_delay;
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
	}
	return 1;
}

int eof_menu_song_seek_last_note(void)
{
	int i;
	unsigned long last_pos = 0;

	if(eof_vocals_selected)
	{
		return eof_menu_song_seek_last_note_vocals();
	}
	else
	{
		if(!eof_music_catalog_playback)
		{
			for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
			{
				if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos > last_pos) && (eof_song->track[eof_selected_track]->note[i]->pos < eof_music_length))
				{
					last_pos = eof_song->track[eof_selected_track]->note[i]->pos;
				}
			}
			alogg_seek_abs_msecs_ogg(eof_music_track, last_pos + eof_av_delay);
			eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_pos = last_pos + eof_av_delay;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
		}
	}
	return 1;
}

int eof_menu_song_seek_previous_note_vocals(void)
{
	int i;

	for(i = eof_song->vocal_track->lyrics - 1; i >= 0; i--)
	{
		if(eof_song->vocal_track->lyric[i]->pos < ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0))
		{
			alogg_seek_abs_msecs_ogg(eof_music_track, eof_song->vocal_track->lyric[i]->pos + eof_av_delay);
			eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_pos = eof_song->vocal_track->lyric[i]->pos + eof_av_delay;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
			break;
		}
	}
	return 1;
}

int eof_menu_song_seek_previous_note(void)
{
	int i;

	if(eof_vocals_selected)
	{
		return eof_menu_song_seek_previous_note_vocals();
	}
	else
	{
		for(i = eof_song->track[eof_selected_track]->notes - 1; i >= 0; i--)
		{
			if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos < ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0)))
			{
				alogg_seek_abs_msecs_ogg(eof_music_track, eof_song->track[eof_selected_track]->note[i]->pos + eof_av_delay);
				eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
				eof_music_pos = eof_song->track[eof_selected_track]->note[i]->pos + eof_av_delay;
				eof_mix_seek(eof_music_pos);
				eof_reset_lyric_preview_lines();
				break;
			}
		}
	}
	return 1;
}

int eof_menu_song_seek_next_note_vocals(void)
{
	int i;

	for(i = 0; i < eof_song->vocal_track->lyrics; i++)
	{
		if((eof_song->vocal_track->lyric[i]->pos < eof_music_length) && (eof_song->vocal_track->lyric[i]->pos > ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0)))
		{
			alogg_seek_abs_msecs_ogg(eof_music_track, eof_song->vocal_track->lyric[i]->pos + eof_av_delay);
			eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_pos = eof_song->vocal_track->lyric[i]->pos + eof_av_delay;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
			break;
		}
	}
	return 1;
}

int eof_menu_song_seek_next_note(void)
{
	int i;

	if(eof_vocals_selected)
	{
		return eof_menu_song_seek_next_note_vocals();
	}
	else
	{
		for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
		{
			if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->pos < eof_music_length) && (eof_song->track[eof_selected_track]->note[i]->pos > ((eof_music_pos - eof_av_delay >= 0) ? eof_music_pos - eof_av_delay : 0)))
			{
				alogg_seek_abs_msecs_ogg(eof_music_track, eof_song->track[eof_selected_track]->note[i]->pos + eof_av_delay);
				eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
				eof_music_pos = eof_song->track[eof_selected_track]->note[i]->pos + eof_av_delay;
				eof_mix_seek(eof_music_pos);
				eof_reset_lyric_preview_lines();
				break;
			}
		}
	}
	return 1;
}

int eof_menu_song_seek_previous_screen(void)
{
	if(!eof_music_catalog_playback)
	{
		if(eof_music_pos - SCREEN_W * eof_zoom < 0)
		{
			eof_menu_song_seek_start();
		}
		else
		{
			alogg_seek_abs_msecs_ogg(eof_music_track, eof_music_pos - SCREEN_W * eof_zoom);
			eof_music_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_actual_pos = eof_music_pos;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
		}
	}
	return 1;
}

int eof_menu_song_seek_next_screen(void)
{
	if(!eof_music_catalog_playback)
	{
		if(eof_music_pos + SCREEN_W * eof_zoom > eof_music_length)
		{
			eof_menu_song_seek_end();
		}
		else
		{
			alogg_seek_abs_msecs_ogg(eof_music_track, eof_music_pos + SCREEN_W * eof_zoom);
			eof_music_pos = alogg_get_pos_msecs_ogg(eof_music_track);
			eof_music_actual_pos = eof_music_pos;
			eof_mix_seek(eof_music_pos);
			eof_reset_lyric_preview_lines();
		}
	}
	return 1;
}

int eof_menu_song_seek_bookmark_help(int b)
{
	if(!eof_music_catalog_playback && (eof_song->bookmark_pos[b] != 0))
	{
		alogg_seek_abs_msecs_ogg(eof_music_track, eof_song->bookmark_pos[b] + eof_av_delay);
		eof_music_pos = eof_song->bookmark_pos[b] + eof_av_delay;
		eof_music_actual_pos = alogg_get_pos_msecs_ogg(eof_music_track);
		eof_mix_seek(eof_music_pos);
		eof_reset_lyric_preview_lines();
		return 1;
	}
	return 0;
}

int eof_menu_song_seek_bookmark_0(void)
{
	eof_menu_song_seek_bookmark_help(0);
	return 1;
}

int eof_menu_song_seek_bookmark_1(void)
{
	eof_menu_song_seek_bookmark_help(1);
	return 1;
}

int eof_menu_song_seek_bookmark_2(void)
{
	eof_menu_song_seek_bookmark_help(2);
	return 1;
}

int eof_menu_song_seek_bookmark_3(void)
{
	eof_menu_song_seek_bookmark_help(3);
	return 1;
}

int eof_menu_song_seek_bookmark_4(void)
{
	eof_menu_song_seek_bookmark_help(4);
	return 1;
}

int eof_menu_song_seek_bookmark_5(void)
{
	eof_menu_song_seek_bookmark_help(5);
	return 1;
}

int eof_menu_song_seek_bookmark_6(void)
{
	eof_menu_song_seek_bookmark_help(6);
	return 1;
}

int eof_menu_song_seek_bookmark_7(void)
{
	eof_menu_song_seek_bookmark_help(7);
	return 1;
}

int eof_menu_song_seek_bookmark_8(void)
{
	eof_menu_song_seek_bookmark_help(8);
	return 1;
}

int eof_menu_song_seek_bookmark_9(void)
{
	eof_menu_song_seek_bookmark_help(9);
	return 1;
}

int eof_menu_song_file_info(void)
{
	allegro_message("Song Folder:\n  %s\n\nEOF Filename:\n  %s\n\nRevision %lu", eof_song_path, eof_loaded_song_name, eof_song->tags->revision);
	return 1;
}

int eof_menu_song_ini_settings(void)
{
	eof_cursor_visible = 0;
	eof_render();
	eof_color_dialog(eof_ini_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_ini_dialog);
	if(eof_popup_dialog(eof_ini_dialog, 0) == 4)
	{
	}
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	eof_show_mouse(NULL);
	return 1;
}

int eof_is_number(char * buffer)
{
	int i;

	for(i = 0; i < ustrlen(buffer); i++)
	{
		if((buffer[i] < '0') || (buffer[i] > '9'))
		{
			return 0;
		}
	}
	return 1;
}

int eof_menu_song_properties(void)
{
//	double bpm = (double)60000000 / (double)eof_song->beat[0]->ppqn;
	int old_offset = eof_song->tags->ogg[eof_selected_ogg].midi_offset;
	int i, invalid = 0;

	eof_cursor_visible = 0;
	if(eof_song_loaded)
	{
		eof_music_paused = 1;
		eof_stop_midi();
		alogg_stop_ogg(eof_music_track);
	}
	eof_render();
	eof_color_dialog(eof_song_properties_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_song_properties_dialog);
	ustrcpy(eof_etext, eof_song->tags->title);
	ustrcpy(eof_etext2, eof_song->tags->artist);
	ustrcpy(eof_etext3, eof_song->tags->frettist);
//	sprintf(eof_etext4, "%f", bpm);
	sprintf(eof_etext4, "%d", eof_song->tags->ogg[eof_selected_ogg].midi_offset);
	ustrcpy(eof_etext5, eof_song->tags->year);
	ustrcpy(eof_etext6, eof_song->tags->loading_text);
	eof_song_properties_dialog[15].flags = eof_song->tags->lyrics ? D_SELECTED : 0;
	eof_song_properties_dialog[16].flags = eof_song->tags->eighth_note_hopo ? D_SELECTED : 0;
	if(eof_popup_dialog(eof_song_properties_dialog, 2) == 17)
	{
		if(ustricmp(eof_song->tags->title, eof_etext) || ustricmp(eof_song->tags->artist, eof_etext2) || ustricmp(eof_song->tags->frettist, eof_etext3) || ustricmp(eof_song->tags->year, eof_etext5) || ustricmp(eof_song->tags->loading_text, eof_etext6))
		{
			eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		}
		else if(eof_is_number(eof_etext4) && (eof_song->tags->ogg[eof_selected_ogg].midi_offset != atol(eof_etext4)))
		{
			eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		}
		else if((eof_song->tags->lyrics && !(eof_song_properties_dialog[15].flags & D_SELECTED)) || (!eof_song->tags->lyrics && (eof_song_properties_dialog[15].flags & D_SELECTED)) || (eof_song->tags->eighth_note_hopo && !(eof_song_properties_dialog[16].flags & D_SELECTED)) || (!eof_song->tags->eighth_note_hopo && (eof_song_properties_dialog[16].flags & D_SELECTED)))
		{
			eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		}
		if((ustrlen(eof_etext) > 255) || (ustrlen(eof_etext2) > 255) || (ustrlen(eof_etext3) > 255) || (ustrlen(eof_etext5) > 31) || (ustrlen(eof_etext6) > 511))
		{
			allegro_message("Text too large for allocated buffer!");
			return 1;
		}
		ustrcpy(eof_song->tags->title, eof_etext);
		ustrcpy(eof_song->tags->artist, eof_etext2);
		ustrcpy(eof_song->tags->frettist, eof_etext3);
		ustrcpy(eof_song->tags->year, eof_etext5);
		ustrcpy(eof_song->tags->loading_text, eof_etext6);
		eof_song->tags->lyrics = (eof_song_properties_dialog[15].flags & D_SELECTED) ? 1 : 0;
		eof_song->tags->eighth_note_hopo = (eof_song_properties_dialog[16].flags & D_SELECTED) ? 1 : 0;
		ustrcpy(eof_last_frettist, eof_etext3);
		if(!eof_is_number(eof_etext4))
		{
			invalid = 1;
		}
		if(!invalid)
		{
			eof_song->tags->ogg[eof_selected_ogg].midi_offset = atol(eof_etext4);
			if(eof_song->tags->ogg[eof_selected_ogg].midi_offset < 0)
			{
				invalid = 1;
			}
		}
		if(invalid)
		{
			allegro_message("Invalid MIDI offset.");
		}
		if(eof_song->beat[0]->pos != eof_song->tags->ogg[eof_selected_ogg].midi_offset)
		{
			for(i = 0; i < eof_song->beats; i++)
			{
				eof_song->beat[i]->fpos += (double)(eof_song->tags->ogg[eof_selected_ogg].midi_offset - old_offset);
				eof_song->beat[i]->pos = eof_song->beat[i]->fpos;
			}
		}
		if((eof_song->tags->ogg[eof_selected_ogg].midi_offset != old_offset) && (eof_count_notes() > 0))
		{
			if(alert(NULL, "Adjust notes to new offset?", NULL, "&Yes", "&No", 'y', 'n') == 1)
			{
				eof_adjust_notes(eof_song->tags->ogg[eof_selected_ogg].midi_offset - old_offset);
			}
			eof_clear_input();
		}
		eof_fixup_notes();
		eof_calculate_beats(eof_song);
		eof_fix_window_title();
	}
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	eof_show_mouse(NULL);
	return 1;
}

int eof_menu_song_test(void)
{
	char syscommand[1024] = {0};
	char temppath[1024] = {0};
	char temppath2[1024] = {0};
	int difficulty = 0;
	int part = 0;

	/* check difficulty before allowing test */
	difficulty = eof_figure_difficulty();
	if(difficulty < 0)
	{
		alert("Error", NULL, "No Notes!", "OK", NULL, 0, KEY_ENTER);
		eof_clear_input();
		return 1;
	}

	/* which part are we going to play */
	part = eof_figure_part();
	if(part < 0)
	{
		alert("Error", NULL, "No Notes!", "OK", NULL, 0, KEY_ENTER);
		eof_clear_input();
		return 1;
	}

	/* switch to songs folder */
	if(eof_chdir(eof_fof_songs_path))
	{
		allegro_message("Song could not be tested!\nMake sure you set the FOF song folder correctly (\"Link To FOF\")!");
		return 1;
	}

	/* create temporary song folder in library */
	eof_mkdir("EOFTemp");

	/* save temporary song */
	ustrcpy(temppath, eof_fof_songs_path);
	ustrcat(temppath, "EOFTemp\\");
	append_filename(temppath2, temppath, "notes.eof", 1024);
	eof_sort_notes();
	eof_fixup_notes();
	if(!eof_save_song(eof_song, temppath2))
	{
		allegro_message("Song could not be tested!\nMake sure you set the FOF song folder correctly (\"Link To FOF\")!");
		get_executable_name(temppath, 1024);
		replace_filename(temppath, temppath, "", 1024);
		eof_chdir(temppath);
		eof_show_mouse(NULL);
		eof_cursor_visible = 1;
		eof_pen_visible = 1;
		return 1;
	}
	append_filename(temppath2, temppath, "notes.mid", 1024);
	eof_export_midi(eof_song, temppath2);
	append_filename(temppath2, temppath, "song.ini", 1024);
	eof_save_ini(eof_song, temppath2);
	sprintf(syscommand, "%sguitar.ogg", eof_song_path);
	sprintf(temppath2, "%sEOFTemp\\guitar.ogg", eof_fof_songs_path);
	eof_copy_file(syscommand, temppath2);

	/* switch to FOF folder */
	replace_filename(temppath, eof_fof_executable_path, "", 1024);
	if(eof_chdir(temppath))
	{
		allegro_message("Song could not be tested!\nMake sure you set the FOF song folder correctly (\"Link To FOF\")!");
		return 1;
	}

	/* execute FOF */
	ustrcpy(syscommand, eof_fof_executable_name);
	ustrcat(syscommand, " -p \"EOFTemp\" -D ");
	sprintf(temppath, "%d", difficulty);
	ustrcat(syscommand, temppath);
	ustrcat(syscommand, " -P ");
	sprintf(temppath, "%d", part);
	ustrcat(syscommand, temppath);
	eof_system(syscommand);

	/* switch to songs folder */
	if(eof_chdir(eof_fof_songs_path))
	{
		allegro_message("Cleanup failed!");
		return 1;
	}

	/* delete temporary song folder */
	delete_file("EOFTemp\\guitar.ogg");
	delete_file("EOFTemp\\notes.mid");
	delete_file("EOFTemp\\song.ini");
	delete_file("EOFTemp\\notes.eof");
	eof_system("rd EOFTemp");

	/* switch back to EOF folder */
	get_executable_name(temppath, 1024);
	replace_filename(temppath, temppath, "", 1024);
	eof_chdir(temppath);


	return 1;
}

int eof_menu_track_selected_guitar(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_GUITAR);
}

int eof_menu_track_selected_bass(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_BASS);
}

int eof_menu_track_selected_guitar_coop(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_GUITAR_COOP);
}

int eof_menu_track_selected_rhythm(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_RHYTHM);
}

int eof_menu_track_selected_drum(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_DRUM);
}

int eof_menu_track_selected_vocals(void)
{
	return eof_menu_track_selected_track_number(EOF_TRACK_VOCALS);
}

int eof_menu_track_selected_track_number(int tracknum)
{
	int i;

	if((tracknum >= 0) && (tracknum <= EOF_MAX_TRACKS))
	{
		for(i = 0; i <= EOF_MAX_TRACKS + 1; i++)
		{
			eof_track_selected_menu[i].flags = 0;
		}

		if(tracknum == EOF_TRACK_VOCALS)
			eof_vocals_selected = 1;
		else
			eof_vocals_selected = 0;

		eof_track_selected_menu[tracknum].flags = D_SELECTED;
		eof_selected_track = tracknum;
		eof_detect_difficulties(eof_song);
		eof_fix_window_title();
	}
	return 1;
}

char * eof_ini_list(int index, int * size)
{
	int i;
	int ecount = 0;
	char * etextpointer[EOF_MAX_INI_SETTINGS] = {NULL};

	if(eof_song->tags->ini_settings >= EOF_MAX_INI_SETTINGS)	//If the maximum number of settings has been met or exceeded
		eof_ini_dialog[2].flags = D_DISABLED;	//Disable the "Add" Song INI dialog button
	else
		eof_ini_dialog[2].flags = 0;	//Enable the "Add" Song INI dialog button

	for(i = 0; i < eof_song->tags->ini_settings; i++)
	{
		if(ecount < EOF_MAX_INI_SETTINGS)
		{
			etextpointer[ecount] = eof_song->tags->ini_setting[i];
			ecount++;
		}
	}
	switch(index)
	{
		case -1:
		{
			*size = ecount;
			if(ecount > 0)
			{
				eof_ini_dialog[3].flags = 0;
			}
			else
			{
				eof_ini_dialog[3].flags = D_DISABLED;	//Disable the "Delete" Song INI dialog button if there are no settings
			}
		}
		default:
		{
			return etextpointer[index];
		}
	}
	return NULL;
}

int eof_menu_catalog_show(void)
{
	if(eof_song->catalog->entries > 0)
	{
		if(eof_catalog_menu[0].flags & D_SELECTED)
		{
			eof_catalog_menu[0].flags = 0;
		}
		else
		{
			eof_catalog_menu[0].flags = D_SELECTED;
			if((eof_song->catalog->entries > 0) && !eof_music_catalog_playback)
			{
				eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
			}
		}
	}
	else
	{
		eof_catalog_menu[0].flags = 0;
	}
	return 1;
}

int eof_menu_catalog_add_vocals(void)
{
	int first_pos = -1;
	int last_pos = -1;
	int i;
	int next;

	for(i = 0; i < eof_song->vocal_track->lyrics; i++)
	{
		if((eof_selection.track == EOF_TRACK_VOCALS) && eof_selection.multi[i])
		{
			if(first_pos == -1)
			{
				first_pos = eof_song->vocal_track->lyric[i]->pos;
			}
			if(eof_song->vocal_track->lyric[i]->length < 100)
			{
				last_pos = eof_song->vocal_track->lyric[i]->pos + 100;
				next = eof_fixup_next_lyric(eof_song->vocal_track, i);
				if(next >= 0)
				{
					if(last_pos >= eof_song->vocal_track->lyric[next]->pos)
					{
						last_pos = eof_song->vocal_track->lyric[next]->pos - 1;
					}
				}
			}
			else
			{
				last_pos = eof_song->vocal_track->lyric[i]->pos + eof_song->vocal_track->lyric[i]->length;
			}
		}
	}
	eof_song->catalog->entry[eof_song->catalog->entries].track = EOF_TRACK_VOCALS;
	eof_song->catalog->entry[eof_song->catalog->entries].type = 0;
	eof_song->catalog->entry[eof_song->catalog->entries].start_pos = first_pos;
	eof_song->catalog->entry[eof_song->catalog->entries].end_pos = last_pos;
	if((eof_song->catalog->entry[eof_song->catalog->entries].start_pos != -1) && (eof_song->catalog->entry[eof_song->catalog->entries].end_pos != -1))
	{
		eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		eof_song->catalog->entries++;
		eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
	}

	return 1;
}

int eof_menu_catalog_add(void)
{
	int first_pos = -1;
	int last_pos = -1;
	int i;
	int next;

	if(eof_vocals_selected)
	{
		return eof_menu_catalog_add_vocals();
	}
	for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
	{
		if((eof_selection.track == eof_selected_track) && eof_selection.multi[i] && (eof_song->track[eof_selected_track]->note[i]->type == eof_note_type))
		{
			if(first_pos == -1)
			{
				first_pos = eof_song->track[eof_selected_track]->note[i]->pos;
			}
			if(eof_song->track[eof_selected_track]->note[i]->length < 100)
			{
				last_pos = eof_song->track[eof_selected_track]->note[i]->pos + 100;
				next = eof_fixup_next_note(eof_song->track[eof_selected_track], i);
				if(next >= 0)
				{
					if(last_pos >= eof_song->track[eof_selected_track]->note[next]->pos)
					{
						last_pos = eof_song->track[eof_selected_track]->note[next]->pos - 1;
					}
				}
			}
			else
			{
				last_pos = eof_song->track[eof_selected_track]->note[i]->pos + eof_song->track[eof_selected_track]->note[i]->length;
			}
		}
	}
	eof_song->catalog->entry[eof_song->catalog->entries].track = eof_selected_track;
	eof_song->catalog->entry[eof_song->catalog->entries].type = eof_note_type;
	eof_song->catalog->entry[eof_song->catalog->entries].start_pos = first_pos;
	eof_song->catalog->entry[eof_song->catalog->entries].end_pos = last_pos;
	if((eof_song->catalog->entry[eof_song->catalog->entries].start_pos != -1) && (eof_song->catalog->entry[eof_song->catalog->entries].end_pos != -1))
	{
		eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		eof_song->catalog->entries++;
		eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
	}

	return 1;
}

int eof_menu_catalog_delete(void)
{
	int i;

	if(eof_song->catalog->entries > 0)
	{
		eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		for(i = eof_selected_catalog_entry; i < eof_song->catalog->entries - 1; i++)
		{
			memcpy(&eof_song->catalog->entry[i], &eof_song->catalog->entry[i + 1], sizeof(EOF_CATALOG_ENTRY));
//			eof_song->catalog->entry[i].start_pos = eof_song->catalog->entry[i + 1].start_pos;
//			eof_song->catalog->entry[i].end_pos = eof_song->catalog->entry[i + 1].end_pos;
		}
		eof_song->catalog->entries--;
		if((eof_selected_catalog_entry >= eof_song->catalog->entries) && (eof_selected_catalog_entry > 0))
		{
			eof_selected_catalog_entry--;
		}
		eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
		if(eof_song->catalog->entries <= 0)
		{
			eof_catalog_menu[0].flags = 0;
		}
	}
	return 1;
}

int eof_menu_catalog_previous(void)
{
	if((eof_song->catalog->entries > 0) && !eof_music_catalog_playback)
	{
		eof_selected_catalog_entry--;
		if(eof_selected_catalog_entry < 0)
		{
			eof_selected_catalog_entry = eof_song->catalog->entries - 1;
		}
		eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
	}
	return 1;
}

int eof_menu_catalog_next(void)
{
	if((eof_song->catalog->entries > 0) && !eof_music_catalog_playback)
	{
		eof_selected_catalog_entry++;
		if(eof_selected_catalog_entry >= eof_song->catalog->entries)
		{
			eof_selected_catalog_entry = 0;
		}
		eof_music_catalog_pos = eof_song->catalog->entry[eof_selected_catalog_entry].start_pos + eof_av_delay;
	}
	return 1;
}

int eof_ini_dialog_add(DIALOG * d)
{
	int i;

	if(eof_song->tags->ini_settings >= EOF_MAX_INI_SETTINGS)	//If the maximum number of INI settings is already defined
		return D_O_K;	//Return without adding anything

	eof_cursor_visible = 0;
	eof_render();
	eof_color_dialog(eof_ini_add_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_ini_add_dialog);
	ustrcpy(eof_etext, "");

	if(eof_popup_dialog(eof_ini_add_dialog, 2) == 3)
	{
		if((ustrlen(eof_etext) > 0) && eof_check_string(eof_etext))
		{
			eof_prepare_undo(EOF_UNDO_TYPE_NONE);
			ustrcpy(eof_song->tags->ini_setting[eof_song->tags->ini_settings], eof_etext);
			eof_song->tags->ini_settings++;
		}
	}

	dialog_message(eof_ini_dialog, MSG_DRAW, 0, &i);
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	eof_show_mouse(screen);
	return D_O_K;
}

int eof_ini_dialog_delete(DIALOG * d)
{
	int i;

	if(eof_song->tags->ini_settings > 0)
	{
		eof_prepare_undo(EOF_UNDO_TYPE_NONE);
		for(i = eof_ini_dialog[1].d1; i < eof_song->tags->ini_settings - 1; i++)
		{
			memcpy(eof_song->tags->ini_setting[i], eof_song->tags->ini_setting[i + 1], 512);
		}
		eof_song->tags->ini_settings--;
		if(eof_ini_dialog[1].d1 >= eof_song->tags->ini_settings)
		{
			eof_ini_dialog[1].d1--;
		}
	}
	dialog_message(eof_ini_dialog, MSG_DRAW, 0, &i);
	return D_O_K;
}

int eof_menu_song_waveform(void)
{
	if(!eof_song_loaded)
		return 1;	//Return error

	if(eof_display_waveform == 0)
	{
		if(eof_music_paused)
		{	//Don't try to generate the waveform data if the chart is playing
			set_window_title("Generating Waveform Graph...");
			if(eof_waveform == NULL)
			{
				eof_waveform = eof_create_waveform(eof_loaded_ogg_name,1);	//Generate 1ms waveform data from the current audio file
			}
			else if(ustricmp(eof_waveform->oggfilename,eof_loaded_ogg_name) != 0)
			{	//If the user opened a different OGG file since the waveform data was generated
				eof_destroy_waveform(eof_waveform);
				eof_waveform = eof_create_waveform(eof_loaded_ogg_name,1);	//Generate 1ms waveform data from the current audio file
			}
		}

		if(eof_waveform != NULL)
		{
			eof_display_waveform = 1;
			eof_waveform_menu[0].flags = D_SELECTED;	//Check the Show item in the Song>Waveform graph menu
		}
	}
	else
	{
		eof_display_waveform = 0;
		eof_waveform_menu[0].flags = 0;	//Clear the Show item in the Song>Waveform graph menu
	}

	if(eof_music_paused)
		eof_render();
	eof_fix_window_title();

	return 0;	//Return success
}

DIALOG eof_leading_silence_dialog[] =
{
   /* (proc) 		        (x)	(y)	(w)	(h)	(fg) (bg) (key) (flags)	(d1)(d2)(dp)						(dp2) (dp3) */
   { d_agup_window_proc,  	  0,	 48,200,248,2,   23,  0,    0,      0,	0,	"Leading Silence",          NULL, NULL },
   { d_agup_text_proc      , 16,     80,110,20, 2,   23,  0,    0,      0,  0,  "Add:",                      NULL, NULL },
   { d_agup_radio_proc,		 16,	100,110,15,	2,   23,  0,    0,      0,	0,	"Milliseconds",             NULL, NULL },
   { d_agup_radio_proc,		 16,	120,110,15,	2,   23,  0,    0,      0,	0,	"Beats",                    NULL, NULL },
   { d_agup_text_proc      , 16,    140,110,20, 2,   23,  0,    0,      0,  0,  "Pad:",                      NULL, NULL },
   { d_agup_radio_proc,		 16,	160,110,15,	2,   23,  0,    0,      0,	0,	"Milliseconds",	            NULL, NULL },
   { d_agup_radio_proc,		 16,	180,110,15,	2,   23,  0,    0,      0,	0,	"Beats",                    NULL, NULL },
   { eof_verified_edit_proc, 16,    200,110,20, 2,   23,  0,    0,    255,  0,   eof_etext,         "1234567890", NULL },
   { d_agup_check_proc,		  16,	226, 180,16,	2,   23,  0,    D_SELECTED,		1,	0,	"Adjust Notes/Beats",		NULL, NULL },
   { d_agup_button_proc,	  16,	252, 68,28,	2,   23,  '\r',	D_EXIT, 0,	0,	"OK",             			NULL, NULL },
   { d_agup_button_proc,	 116,252,68, 28,	2,   23,  0,	D_EXIT, 0,	0,	"Cancel",         			NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

static long get_ogg_length(const char * fn)
{
	ALOGG_OGG * ogg;
	unsigned long length = 0;
	FILE * fp = fopen(fn, "rb");
	if(fp)
	{
		ogg = alogg_create_ogg_from_file(fp);
		length = alogg_get_length_msecs_ogg(ogg);
		alogg_destroy_ogg(ogg);
	}
	return length;
}

int eof_menu_song_add_silence(void)
{
	double beat_length;
	unsigned long silence_length = 0;
	long current_length = 0;
	long after_silence_length = 0;
	long adjust = 0;
	int i, x;
	char fn[1024] = {0};

	eof_cursor_visible = 0;
	eof_pen_visible = 0;
	eof_render();
	eof_color_dialog(eof_leading_silence_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_leading_silence_dialog);

	for(x=1;x<=4;x++)
	{
		eof_leading_silence_dialog[x].flags = 0;
	}
	eof_leading_silence_dialog[2].flags = D_SELECTED;
	eof_leading_silence_dialog[3].flags = 0;
	eof_leading_silence_dialog[5].flags = 0;
	eof_leading_silence_dialog[6].flags = 0;
	sprintf(eof_etext, "0");

	if(eof_popup_dialog(eof_leading_silence_dialog, 0) == 9)			//User clicked OK
	{
		sprintf(fn, "%s.backup", eof_loaded_ogg_name);
		current_length = get_ogg_length(eof_loaded_ogg_name);
		/* revert to original file */
		eof_prepare_undo(EOF_UNDO_TYPE_SILENCE);
		if(atoi(eof_etext) <= 0)
		{
			eof_copy_file(fn, eof_loaded_ogg_name);
			if(eof_load_ogg(eof_loaded_ogg_name))
			{
				eof_fix_waveform_graph();
				eof_fix_window_title();
			}
		}
		else
		{
			if(eof_leading_silence_dialog[2].flags & D_SELECTED)
			{
				silence_length = atoi(eof_etext);
			}
			else if(eof_leading_silence_dialog[3].flags & D_SELECTED)
			{
				beat_length = (double)60000 / ((double)60000000.0 / (double)eof_song->beat[0]->ppqn);
				silence_length = beat_length * (double)atoi(eof_etext);
			}
			else if(eof_leading_silence_dialog[5].flags & D_SELECTED)
			{
				silence_length = atoi(eof_etext);
				if(silence_length > eof_song->beat[0]->pos)
				{
					silence_length -= eof_song->beat[0]->pos;
				}
			}
			else if(eof_leading_silence_dialog[6].flags & D_SELECTED)
			{
				beat_length = (double)60000 / ((double)60000000.0 / (double)eof_song->beat[0]->ppqn);
				silence_length = beat_length * (double)atoi(eof_etext);
				printf("%lu\n", silence_length);
				if(silence_length > eof_song->beat[0]->pos)
				{
					silence_length -= eof_song->beat[0]->pos;
				}
			}
			eof_add_silence(eof_loaded_ogg_name, silence_length);
			after_silence_length = get_ogg_length(eof_loaded_ogg_name);
		}

		/* adjust notes/beats */
		if(eof_leading_silence_dialog[8].flags & D_SELECTED)
		{
			if(after_silence_length != 0)
			{
				adjust = after_silence_length - current_length;
			}
			else
			{
				adjust = get_ogg_length(fn) - current_length;
			}
			if(eof_song->tags->ogg[eof_selected_ogg].midi_offset + adjust < 0)
			{
				adjust = 0;
			}
			eof_song->tags->ogg[eof_selected_ogg].midi_offset += adjust;
			if(eof_song->beat[0]->pos != eof_song->tags->ogg[eof_selected_ogg].midi_offset)
			{
				for(i = 0; i < eof_song->beats; i++)
				{
					eof_song->beat[i]->fpos += (double)adjust;
					eof_song->beat[i]->pos = eof_song->beat[i]->fpos;
				}
			}
			eof_adjust_notes(adjust);
		}
		eof_fixup_notes();
		eof_calculate_beats(eof_song);
		eof_fix_window_title();
	}
	eof_show_mouse(NULL);
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	return 1;
}

char eof_chart_volume_string[10] = "100%";
char eof_clap_volume_string[10] = "100%";
char eof_metronome_volume_string[10] = "100%";
char eof_tone_volume_string[10] = "100%";
char eof_percussion_volume_string[10] = "100%";
DIALOG eof_audio_cues_dialog[] =
{
/*	(proc)					(x)	(y)				(w)	(h)		(fg)	(bg)	(key)	(flags)	(d1)	(d2)	(dp)							(dp2)	(dp3) */
	{ d_agup_window_proc,	0,	48,				310,310,	2,		23,		0,		0,		0,		0,		"Audio cues",					NULL,	NULL },
	{ d_agup_text_proc,		16,	88,				64,	8,		2,		23,		0,		0,		0,		0,		"Chart volume",					NULL,	NULL },
	{ d_agup_slider_proc,	176,88,				96,	16,		2,		23,		0,		0,		100,	0,		NULL,							eof_set_cue_volume,	eof_chart_volume_string },
	{ d_agup_text_proc,		275,88,				30,	16,		2,		23,		0,		0,		100,	0,		eof_chart_volume_string,		NULL,	NULL },
	{ d_agup_text_proc,		16,	108,			64,	8,		2,		23,		0,		0,		0,		0,		"Clap volume",					NULL,	NULL },
	{ d_agup_slider_proc,	176,108,			96,	16,		2,		23,		0,		0,		100,	0,		NULL,							eof_set_cue_volume,	eof_clap_volume_string },
	{ d_agup_text_proc,		275,108,			30,	16,		2,		23,		0,		0,		100,	0,		eof_clap_volume_string,			NULL,	NULL },
	{ d_agup_text_proc,		16, 128,			64,	8,		2,		23,		0,		0,		0,		0,		"Metronome volume",				NULL,	NULL },
	{ d_agup_slider_proc,	176,128,			96,	16,		2,		23,		0,		0,		100,	0,		NULL,							eof_set_cue_volume,	eof_metronome_volume_string },
	{ d_agup_text_proc,		275,128,			30,	16,		2,		23,		0,		0,		100,	0,		eof_metronome_volume_string,	NULL,	NULL },
	{ d_agup_text_proc,		16, 148,			64,	8,		2,		23,		0,		0,		0,		0,		"Tone volume",					NULL,	NULL },
	{ d_agup_slider_proc,	176,148,			96,	16,		2,		23,		0,		0,		100,	0,		NULL,							eof_set_cue_volume,	eof_tone_volume_string },
	{ d_agup_text_proc,		275,148,			30,	16,		2,		23,		0,		0,		100,	0,		eof_tone_volume_string,			NULL,	NULL },
	{ d_agup_text_proc,		16, 168,			64,	8,		2,		23,		0,		0,		0,		0,		"Vocal Percussion volume",		NULL,	NULL },
	{ d_agup_slider_proc,	176,168,			96,	16,		2,		23,		0,		0,		100,	0,		NULL,							eof_set_cue_volume,	eof_percussion_volume_string },
	{ d_agup_text_proc,		275,168,			30,	16,		2,		23,		0,		0,		100,	0,		eof_percussion_volume_string,	NULL,	NULL },
	{ d_agup_text_proc,		16, 188,			64,	8,		2,		23,		0,		0,		0,		0,		"Vocal Percussion sound:",		NULL,	NULL },
	{ d_agup_radio_proc,	16,208,				68,	15,		2,		23,		0,		0,		0,		0,		"Cowbell",						NULL,	NULL },
	{ d_agup_radio_proc,	124,208,			84,	15,		2,		23,		0,		0,		0,		0,		"Triangle 1",					NULL,	NULL },
	{ d_agup_radio_proc,	210,208,			30,	15,		2,		23,		0,		0,		0,		0,		"2",							NULL,	NULL },
	{ d_agup_radio_proc,	16,228,				102,15,		2,		23,		0,		0,		0,		0,		"Tambourine 1",					NULL,	NULL },
	{ d_agup_radio_proc,	124,228,			30,	15,		2,		23,		0,		0,		0,		0,		"2",							NULL,	NULL },
	{ d_agup_radio_proc,	160,228,			30,	15,		2,		23,		0,		0,		0,		0,		"3",							NULL,	NULL },
	{ d_agup_radio_proc,	16,248,				110,15,		2,		23,		0,		0,		0,		0,		"Wood Block 1",					NULL,	NULL },
	{ d_agup_radio_proc,	124,248,			30,	15,		2,		23,		0,		0,		0,		0,		"2",							NULL,	NULL },
	{ d_agup_radio_proc,	160,248,			30,	15,		2,		23,		0,		0,		0,		0,		"3",							NULL,	NULL },
	{ d_agup_radio_proc,	196,248,			30,	15,		2,		23,		0,		0,		0,		0,		"4",							NULL,	NULL },
	{ d_agup_radio_proc,	16,268,				30,	15,		2,		23,		0,		0,		0,		0,		"5",							NULL,	NULL },
	{ d_agup_radio_proc,	52,268,				30,	15,		2,		23,		0,		0,		0,		0,		"6",							NULL,	NULL },
	{ d_agup_radio_proc,	88,268,				30,	15,		2,		23,		0,		0,		0,		0,		"7",							NULL,	NULL },
	{ d_agup_radio_proc,	124,268,			30,	15,		2,		23,		0,		0,		0,		0,		"8",							NULL,	NULL },
	{ d_agup_radio_proc,	160,268,			30,	15,		2,		23,		0,		0,		0,		0,		"9",							NULL,	NULL },
	{ d_agup_radio_proc,	196,268,			40,	15,		2,		23,		0,		0,		0,		0,		"10",							NULL,	NULL },
	{ d_agup_radio_proc,	16,288,				68,15,		2,		23,		0,		0,		0,		0,		"Clap 1",						NULL,	NULL },
	{ d_agup_radio_proc,	88,288,				30,15,		2,		23,		0,		0,		0,		0,		"2",							NULL,	NULL },
	{ d_agup_radio_proc,	124,288,			30,15,		2,		23,		0,		0,		0,		0,		"3",							NULL,	NULL },
	{ d_agup_radio_proc,	160,288,			30,15,		2,		23,		0,		0,		0,		0,		"4",							NULL,	NULL },
	{ d_agup_button_proc,	44,314,				68,	28,		2,		23,		'\r',	D_EXIT,	0,		0,		"OK",							NULL,	NULL },
	{ d_agup_button_proc,	172,314,			68,	28,		2,		23,		0,		D_EXIT,	0,		0,		"Cancel",						NULL,	NULL },
	{ NULL,					0,	0,				0,	0,		0,		0,		0,		0,		0,		0,		NULL,							NULL,	NULL }
};

int eof_set_cue_volume(void *dp3, int d2)
{
	//Validate input
	if(dp3 == NULL)
		return 1;
	if((d2 < 0) || (d2 > 100))
		return 1;

	sprintf((char *)dp3,"%3d%%",d2);	//Rewrite the specified volume slider string
	object_message(&eof_audio_cues_dialog[3], MSG_DRAW, 0);	//Have Allegro redraw the volume slider strings
	object_message(&eof_audio_cues_dialog[6], MSG_DRAW, 0);
	object_message(&eof_audio_cues_dialog[9], MSG_DRAW, 0);
	object_message(&eof_audio_cues_dialog[12], MSG_DRAW, 0);
	object_message(&eof_audio_cues_dialog[15], MSG_DRAW, 0);
	return 0;
}

int eof_menu_audio_cues(void)
{
	int x;

	eof_cursor_visible = 0;
	eof_pen_visible = 0;
	eof_render();
	eof_color_dialog(eof_audio_cues_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_audio_cues_dialog);
	eof_audio_cues_dialog[2].d2 = eof_chart_volume;
	eof_audio_cues_dialog[5].d2 = eof_clap_volume;
	eof_audio_cues_dialog[8].d2 = eof_tick_volume;
	eof_audio_cues_dialog[11].d2 = eof_tone_volume;
	eof_audio_cues_dialog[14].d2 = eof_percussion_volume;

	for(x=17;x<=36;x++)
	{	//Deselect all vocal percussion radio buttons
		eof_audio_cues_dialog[x].flags = 0;
	}
	eof_audio_cues_dialog[eof_selected_percussion_cue].flags = D_SELECTED;	//Activate the radio button for the current vocal percussion cue

	if(eof_popup_dialog(eof_audio_cues_dialog, 0) == 37)			//User clicked OK
	{
		eof_chart_volume = eof_audio_cues_dialog[2].d2;				//Store the volume set by the chart volume slider
		eof_chart_volume_multiplier = sqrt(eof_chart_volume/100.0);	//Store this math so it only needs to be performed once
		eof_clap_volume = eof_audio_cues_dialog[5].d2;				//Store the volume set by the clap cue volume slider
		eof_tick_volume = eof_audio_cues_dialog[8].d2;				//Store the volume set by the tick cue volume slider
		eof_tone_volume = eof_audio_cues_dialog[11].d2;				//Store the volume set by the tone cue volume slider
		eof_percussion_volume = eof_audio_cues_dialog[14].d2;		//Store the volume set by the vocal percussion cue volume slider

		if(eof_audio_cues_dialog[17].flags == D_SELECTED)		//User selected cowbell
		{
			eof_sound_chosen_percussion = eof_sound_cowbell;
			eof_selected_percussion_cue = 17;
		}
		else if(eof_audio_cues_dialog[18].flags == D_SELECTED)	//User selected triangle 1
		{
			eof_sound_chosen_percussion = eof_sound_triangle1;
			eof_selected_percussion_cue = 18;
		}
		else if(eof_audio_cues_dialog[19].flags == D_SELECTED)	//User selected triangle 2
		{
			eof_sound_chosen_percussion = eof_sound_triangle2;
			eof_selected_percussion_cue = 19;
		}
		else if(eof_audio_cues_dialog[20].flags == D_SELECTED)	//User selected tambourine 1
		{
			eof_sound_chosen_percussion = eof_sound_tambourine1;
			eof_selected_percussion_cue = 20;
		}
		else if(eof_audio_cues_dialog[21].flags == D_SELECTED)	//User selected tambourine 2
		{
			eof_sound_chosen_percussion = eof_sound_tambourine2;
			eof_selected_percussion_cue = 21;
		}
		else if(eof_audio_cues_dialog[22].flags == D_SELECTED)	//User selected tambourine 3
		{
			eof_sound_chosen_percussion = eof_sound_tambourine3;
			eof_selected_percussion_cue = 22;
		}
		else if(eof_audio_cues_dialog[23].flags == D_SELECTED)	//User selected wood block 1
		{
			eof_sound_chosen_percussion = eof_sound_woodblock1;
			eof_selected_percussion_cue = 23;
		}
		else if(eof_audio_cues_dialog[24].flags == D_SELECTED)	//User selected wood block 2
		{
			eof_sound_chosen_percussion = eof_sound_woodblock2;
			eof_selected_percussion_cue = 24;
		}
		else if(eof_audio_cues_dialog[25].flags == D_SELECTED)	//User selected wood block 3
		{
			eof_sound_chosen_percussion = eof_sound_woodblock3;
			eof_selected_percussion_cue = 25;
		}
		else if(eof_audio_cues_dialog[26].flags == D_SELECTED)	//User selected wood block 4
		{
			eof_sound_chosen_percussion = eof_sound_woodblock4;
			eof_selected_percussion_cue = 26;
		}
		else if(eof_audio_cues_dialog[27].flags == D_SELECTED)	//User selected wood block 5
		{
			eof_sound_chosen_percussion = eof_sound_woodblock5;
			eof_selected_percussion_cue = 27;
		}
		else if(eof_audio_cues_dialog[28].flags == D_SELECTED)	//User selected wood block 6
		{
			eof_sound_chosen_percussion = eof_sound_woodblock6;
			eof_selected_percussion_cue = 28;
		}
		else if(eof_audio_cues_dialog[29].flags == D_SELECTED)	//User selected wood block 7
		{
			eof_sound_chosen_percussion = eof_sound_woodblock7;
			eof_selected_percussion_cue = 29;
		}
		else if(eof_audio_cues_dialog[30].flags == D_SELECTED)	//User selected wood block 8
		{
			eof_sound_chosen_percussion = eof_sound_woodblock8;
			eof_selected_percussion_cue = 30;
		}
		else if(eof_audio_cues_dialog[31].flags == D_SELECTED)	//User selected wood block 9
		{
			eof_sound_chosen_percussion = eof_sound_woodblock9;
			eof_selected_percussion_cue = 31;
		}
		else if(eof_audio_cues_dialog[32].flags == D_SELECTED)	//User selected wood block 10
		{
			eof_sound_chosen_percussion = eof_sound_woodblock10;
			eof_selected_percussion_cue = 32;
		}
		else if(eof_audio_cues_dialog[33].flags == D_SELECTED)	//User selected clap 1
		{
			eof_sound_chosen_percussion = eof_sound_clap1;
			eof_selected_percussion_cue = 33;
		}
		else if(eof_audio_cues_dialog[34].flags == D_SELECTED)	//User selected clap 2
		{
			eof_sound_chosen_percussion = eof_sound_clap2;
			eof_selected_percussion_cue = 34;
		}
		else if(eof_audio_cues_dialog[35].flags == D_SELECTED)	//User selected clap 3
		{
			eof_sound_chosen_percussion = eof_sound_clap3;
			eof_selected_percussion_cue = 35;
		}
		else if(eof_audio_cues_dialog[36].flags == D_SELECTED)	//User selected clap 4
		{
			eof_sound_chosen_percussion = eof_sound_clap4;
			eof_selected_percussion_cue = 36;
		}
	}
	eof_show_mouse(NULL);
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	return 1;
}

DIALOG eof_waveform_settings_dialog[] =
{
   /* (proc) 		        (x)	(y)	(w)	(h)	(fg) (bg) (key) (flags)	(d1)(d2)(dp)						(dp2) (dp3) */
   { d_agup_window_proc,  	0,	48,	200,200,2,   23,  0,    0,      0,	0,	"Configure Waveform Graph",	NULL, NULL },
   { d_agup_text_proc,		16,	80,	64,	8,	2,   23,  0,    0,      0,	0,	"Fit into:",				NULL, NULL },
   { d_agup_radio_proc,		16,	100,110,15,	2,   23,  0,    0,      0,	0,	"Fretboard area",			NULL, NULL },
   { d_agup_radio_proc,		16,	120,110,15,	2,   23,  0,    0,      0,	0,	"Editor window",			NULL, NULL },
   { d_agup_text_proc,		16,	140,80,16,	2,   23,  0,    0,		1,	0,	"Display channels:",		NULL, NULL },
   { d_agup_check_proc,		16,	160,45,16,	2,   23,  0,    0,		1,	0,	"Left",						NULL, NULL },
   { d_agup_check_proc,		16,	180,55,16,	2,   23,  0,    0,		1,	0,	"Right",					NULL, NULL },
   { d_agup_button_proc,	16,	208,68,	28,	2,   23,  '\r',	D_EXIT, 0,	0,	"OK",             			NULL, NULL },
   { d_agup_button_proc,	116,208,68,	28,	2,   23,  0,	D_EXIT, 0,	0,	"Cancel",         			NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

int eof_menu_song_waveform_settings(void)
{
	eof_cursor_visible = 0;
	eof_pen_visible = 0;
	eof_render();
	eof_color_dialog(eof_waveform_settings_dialog, gui_fg_color, gui_bg_color);
	centre_dialog(eof_waveform_settings_dialog);

	eof_waveform_settings_dialog[2].flags = eof_waveform_settings_dialog[3].flags = 0;
	if(eof_waveform_renderlocation == 0)
	{	//If fit into fretboard is active
		eof_waveform_settings_dialog[2].flags = D_SELECTED;
	}
	else
	{	//If fit into editor window is active
		eof_waveform_settings_dialog[3].flags = D_SELECTED;
	}

	eof_waveform_settings_dialog[5].flags = eof_waveform_settings_dialog[6].flags = 0;
	if(eof_waveform_renderleftchannel)
	{	//If the left channel is selected to be rendered
		eof_waveform_settings_dialog[5].flags = D_SELECTED;
	}
	if(eof_waveform_renderrightchannel)
	{	//If the left channel is selected to be rendered
		eof_waveform_settings_dialog[6].flags = D_SELECTED;
	}

	if(eof_popup_dialog(eof_waveform_settings_dialog, 0) == 7)		//User clicked OK
	{
		if(eof_waveform_settings_dialog[2].flags == D_SELECTED)
		{	//User selected to render into fretboard area
			eof_waveform_renderlocation = 0;
		}
		else
		{	//User selected to render into editor window
			eof_waveform_renderlocation = 1;
		}
		if(eof_waveform_settings_dialog[5].flags == D_SELECTED)
		{	//User selected to render the left channel
			eof_waveform_renderleftchannel = 1;
		}
		else
		{
			eof_waveform_renderleftchannel = 0;
		}
		if(eof_waveform_settings_dialog[6].flags == D_SELECTED)
		{	//User selected to render the right channel
			eof_waveform_renderrightchannel = 1;
		}
		else
		{
			eof_waveform_renderrightchannel = 0;
		}
	}
	eof_show_mouse(NULL);
	eof_cursor_visible = 1;
	eof_pen_visible = 1;
	return 1;
}
