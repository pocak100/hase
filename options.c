#include "options.h"
#include "window.h"

int op_zoom = 1;
int op_circle = 1;
int op_music_volume = SP_VOLUME_MAX << VOLUME_SHIFT-1;
int op_sample_volume = SP_VOLUME_MAX << VOLUME_SHIFT;
int op_particles = 2;

int gop_zoom()
{
	return op_zoom;
}

int gop_circle()
{
	return op_circle;
}

int gop_music_volume()
{
	return op_music_volume;
}

int gop_sample_volume()
{
	return op_sample_volume;
}

int gop_particles()
{
	return op_particles;
}

void sop_zoom(int v)
{
	op_zoom = v & 1;
}

void sop_circle(int v)
{
	op_circle = v & 1;
}

void sop_music_volume(int v)
{
	if (v < 0)
		v = 0;
	if (v > (SP_VOLUME_MAX << VOLUME_SHIFT))
		v = SP_VOLUME_MAX << VOLUME_SHIFT;
	op_music_volume = v;
	spSoundSetMusicVolume(v >> VOLUME_SHIFT+3);
}

void sop_sample_volume(int v)
{
	if (v < 0)
		v = 0;
	if (v > (SP_VOLUME_MAX << VOLUME_SHIFT))
		v = SP_VOLUME_MAX << VOLUME_SHIFT;
	op_sample_volume = v;
	spSoundSetVolume(v >> VOLUME_SHIFT);
}

void sop_particles(int v)
{
	if (v < 1)
		v = 1;
	if (v > 4)
		v = 4;
	op_particles = v;
}	

void load_options()
{
	spConfigPointer conf = spConfigRead("config.ini","hase");
	spConfigEntryPointer entry = conf->firstEntry;
	while (entry)
	{
		if (strcmp(entry->key,"zoom") == 0)
			sop_zoom(atoi(entry->value));
		if (strcmp(entry->key,"circle") == 0)
			sop_circle(atoi(entry->value));
		if (strcmp(entry->key,"music_volume") == 0)
			sop_music_volume(atoi(entry->value));
		if (strcmp(entry->key,"sample_volume") == 0)
			sop_sample_volume(atoi(entry->value));
		if (strcmp(entry->key,"particles") == 0)
			sop_particles(atoi(entry->value));
		entry = entry->next;
	}
	spConfigFree(conf);
}

void save_options()
{
	spConfigPointer conf = spConfigRead("config.ini","hase");
	spConfigSetInt(conf,"zoom",op_zoom);
	spConfigSetInt(conf,"circle",op_circle);
	spConfigSetInt(conf,"music_volume",op_music_volume);
	spConfigSetInt(conf,"sample_volume",op_sample_volume);
	spConfigSetInt(conf,"particles",op_particles);
	spConfigWrite(conf);
	spConfigFree(conf);
}

int options_feedback( pWindowElement elem, int action )
{
	switch (action)
	{
		case WN_ACT_LEFT:
			switch (elem->reference)
			{
				case 1:
					sop_zoom(1-gop_zoom());
					break;
				case 2:
					sop_circle(1-gop_circle());
					break;
				case 3:
					sop_music_volume(gop_music_volume()-1);
					break;
				case 4:
					sop_sample_volume(gop_sample_volume()-1);
					break;
				case 5:
					sop_particles(gop_particles()-1);
					break;
			}
			break;
		case WN_ACT_RIGHT:
			switch (elem->reference)
			{
				case 1:
					sop_zoom(1-gop_zoom());
					break;
				case 2:
					sop_circle(1-gop_circle());
					break;
				case 3:
					sop_music_volume(gop_music_volume()+1);
					break;
				case 4:
					sop_sample_volume(gop_sample_volume()+1);
					break;
				case 5:
					sop_particles(gop_particles()+1);
					break;
			}
			break;
	}
	if (elem->type != 2 || action == WN_ACT_UPDATE)
	switch (elem->reference)
	{
		case 1:
			if (gop_zoom())
				sprintf(elem->text,"Locking zoom: Yes");
			else
				sprintf(elem->text,"Locking zoom: No");
			break;
		case 2:
			if (gop_circle())
				sprintf(elem->text,"Mark active hare: Yes");
			else
				sprintf(elem->text,"Mark active hare: No");
			break;
		case 3:
			sprintf(elem->text,"Music volume: %i%%",gop_music_volume()*100/SP_VOLUME_MAX >> VOLUME_SHIFT);
			break;
		case 4:
			sprintf(elem->text,"Effect volume: %i%%",gop_sample_volume()*100/SP_VOLUME_MAX >> VOLUME_SHIFT);
			break;
		case 5:
			switch (gop_particles())
			{
				case 1:
					sprintf(elem->text,"Particles: Very much");
					break;
				case 2:
					sprintf(elem->text,"Particles: Much");
					break;
				case 3:
					sprintf(elem->text,"Particles: Not so much");
					break;
				case 4:
					sprintf(elem->text,"Particles: No");
					break;
			}
			break;
		case 6:
			sprintf(elem->text,"Quit game");
			break;
	}
	return 0;
}

int options_window(spFontPointer font, void ( *resize )( Uint16 w, Uint16 h ),int quit)
{
	pWindow window = create_window(options_feedback,font,"Options");
	window->only_ok = 1;
	add_window_element(window,0,1);
	add_window_element(window,0,2);
	add_window_element(window,2,3);
	add_window_element(window,2,4);
	add_window_element(window,0,5);
	if (quit)
		add_window_element(window,-1,6);
	int res = modal_window(window,resize);
	int ret = 0;
	if (window->selection == 5)
		ret = 1;
	delete_window(window);
	save_options();
	return ret;
}
