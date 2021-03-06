#include "mpdcontrol.c"
#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

#define NUMCOLORS 4

#define normBg "#282a36"
#define normFg "#6272a4"
#define selFg "#f8f8f2"
#define selBg "#bd93f9"
#define urgentBg "#AB4642"
#define font "Terminus:size=11"
#define icon_font "stlarch:size=11"

static const char *fonts[]            = { font, icon_font };

static const char *colors[][3]      = {
	/*		   fg         bg          border   */
	[SchemeNorm] = { selFg, normBg,  normFg },
	[SchemeSel]  = { selFg, normBg,   selFg },
	[SchemeStatus] = { selFg, normBg, normFg },
	[SchemeTagsSel]  = { selFg, selBg, normFg },
	[SchemeTagsNorm]  = { normFg, normBg, normFg },
	[SchemeTagsUrgent]  = { selFg, urgentBg, normFg },
	[SchemeInfoSel]  = { selFg, normBg, normFg },
	[SchemeInfoNorm]  = { selFg, normBg, normFg },
};

static Bool resizehints = False;
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int gappx = 10; // Gap pixel between windows
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */
static const int vertpad            = 0;       /* vertical padding of bar */
static const int sidepad            = 0;       /* horizontal padding of bar */
static const int horizpadbar        = 0;        /* horizontal padding for statusbar */
static const int vertpadbar         = 8;        /* vertical padding for statusbar */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            False,       -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY|ControlMask,           KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY,                       KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} },

static char dmenumon[2] = "0";
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", normBg, "-nf", selFg, "-sb",  selBg, "-sf", selFg, NULL };
static const char *termcmd[] = { "xterm", NULL };
static const char *browsercmd[] = { "firefox", NULL };
static const char *brightnessUpCmd[] = { "xbacklight", "-inc", "10", NULL };
static const char *brightnessDownCmd[] = { "xbacklight", "-dec", "10", NULL };
static const char *lockCmd[] = { "slock", "sudo", "zzz", NULL };
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle",  NULL };
static const char *mutemic[] = { "/usr/bin/pactl", "set-source-mute", "@DEFAULT_SOURCE@", "toggle",  NULL };


static Key keys[] = {
	/* modifier                     key                       function        argument */
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,          {.v = brightnessUpCmd} },
	{ 0,                            XF86XK_MonBrightnessDown, spawn,          {.v = brightnessDownCmd} },
	{ 0,                            XF86XK_AudioMicMute,      spawn,          {.v = mutemic } },
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,          {.v = downvol } },
	{ 0,                            XF86XK_AudioMute,         spawn,          {.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,          {.v = upvol   } },
	{ MODKEY|ShiftMask,             XK_Return,                spawn,          {.v = termcmd } },
	{ MODKEY|ControlMask,           XK_q,	                  spawn,          {.v = lockCmd } },
	{ MODKEY|ShiftMask,             XK_b,                     togglebar,      {0} },
	{ MODKEY,                       XK_m,                     spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_b,                     spawn,          {.v = browsercmd } },
	{ MODKEY,                       XK_F1,                    mpdchange,      {.i = -1} },
	{ MODKEY,                       XK_F2,                    mpdchange,      {.i = +1} },
	{ MODKEY,                       XK_Escape,                mpdcontrol,     {0} },
	{ MODKEY,                       XK_Tab,                   focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                     focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                     incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,                     incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_l,			  setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Return,                zoom,           {0} },
	{ MODKEY,                       XK_o,                     view,           {0} },
	{ MODKEY|ShiftMask,             XK_x,                     killclient,     {0} },
	{ MODKEY,                       XK_space,                 setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,                 togglefloating, {0} },
	{ MODKEY,                       XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                     tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                                     0)
	TAGKEYS(                        XK_2,                                     1)
	TAGKEYS(                        XK_3,                                     2)
	TAGKEYS(                        XK_4,                                     3)
	TAGKEYS(                        XK_5,                                     4)
	TAGKEYS(                        XK_6,                                     5)
	TAGKEYS(                        XK_7,                                     6)
	TAGKEYS(                        XK_8,                                     7)
	TAGKEYS(                        XK_9,                                     8)
	{ MODKEY|ShiftMask,             XK_q,                     quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
