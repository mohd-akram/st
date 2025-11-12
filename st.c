#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL3/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "font.h"

const char *names[] = {
	"sun",
	"earth",
	"ariel",
	"callisto",
	"moon",
	"deimos",
	"dione",
	"enceladus",
	"europa",
	"ganymede",
	"hyperion",
	"iapetus",
	"io",
	"jupiter",
	"mars",
	"mercury",
	"mimas",
	"miranda",
	"neptune",
	"nereid",
	"oberon",
	"phobos",
	"phoebe",
	"pluto",
	"rhea",
	"saturn",
	"tethys",
	"titan",
	"triton",
	"umbriel",
	"uranus",
	"venus"
};

enum bri { br0, br1, br2, br3 };

// Planet brightness
const enum bri points[] = {
	br3,
	br2,
	br0,
	br2,
	br1,
	br0,
	br1,
	br0,
	br1,
	br2,
	br0,
	br1,
	br1,
	br3,
	br2,
	br1,
	br0,
	br0,
	br3,
	br0,
	br1,
	br0,
	br0,
	br1,
	br1,
	br3,
	br1,
	br2,
	br1,
	br0,
	br3,
	br2
};

// Planet orbit index
const int ppar[] = {
	0,0,036,015,01,016,031,031,
	015,015,031,031,015,0,0,0,
	031,036,0,022,036,016,031,0,
	031,0,031,031,022,036,0,0
};

// format for hand-rolled floating point used in PDP-7 version
struct flt {
	char exp;	// exponent
	int m1;		// signed 36-bit mantissa (0-18)
	int m2;		// 18-36
};

/* All values use earth's radius (R_E) as the base distance unit */

// Planet radius squared
double prsq[32];
const struct flt prsq_[] = {
	{016,0272245,075341},
	{1,0200000,0},
	{-07,0244122,0506362},
	{-02,0251477,0620663},
	{-03,0230761,0127762},
	{-025,0320300,054474},
	{-06,0324134,0124211},
	{-010,0335416,0541570},
	{-04,0371372,0},
	{-06,0247430,0},
	{-011,0311150,0},
	{-05,0302622,0},
	{-02,0256475,0},
	{07,0376733,0},
	{-01,0221530,0},
	{-02,0235142,0},
	{-010,0217266,0},
	{-011,0274361,0},
	{04,0365471,0},
	{-012,0227176,0},
	{-06,0342454,0},
	{-023,0326340,0},
	{-013,0326265,0},
	{-02,0323774,0},
	{-05,0255140,0},
	{07,0263573,0},
	{-06,0223174,0},
	{-02,0251477,0},
	{-02,0235142,0},
	{-05,0223060,0},
	{05,0206115,0},
	{0,0362406,0}
};

// Planet GM values
double accl[32];
const struct flt accl_[] = {
	{0,0204365,0},
	{-023,0320324,0},
	{-036,0227207,0},
	{-017,0340500,0},
	{-030,0210041,0},
	{-063,0341666,0},
	{-034,0235122,0},
	{-037,0247531,0},
	{-031,0310316,0},
	{-027,0334427,0},
	{-041,0315203,0},
	{-033,0303403,0},
	{-030,0245752,0},
	{-017,0201414,0},
	{-026,0263753,0},
	{-026,0205241,0},
	{-040,0256464,0},
	{-041,0272051,0},
	{-017,0340566,0},
	{-043,0275073,0},
	{-034,0255345,0},
	{-060,0341650,0},
	{-044,0341552,0},
	{-020,0307762,0},
	{-033,0243712,0},
	{-014,0233053,0},
	{-035,0265543,0},
	{-027,0340500,0},
	{-027,0210344,0},
	{-037,0210777,0},
	{-017,0275653,0},
	{-023,0252667,0}
};

// Planet x position from orbit center
double px[32];
const struct flt px_[] = {
	{0,000000,0},
	{015,0620356,0},
	{005,0360005,0},
	{010,0666214,0},
	{005,0704053,0},
	{002,0347600,0},
	{005,0310506,0},
	{006,0220622,0},
	{007,0310473,0},
	{004,0370065,0},
	{006,0304101,0},
	{011,0676631,0},
	{006,0653020,0},
	{020,0317202,0},
	{017,0644356,0},
	{013,0206414,0},
	{005,0245346,0},
	{004,0222264,0},
	{023,0261234,0},
	{001,0372225,0},
	{007,0646102,0},
	{001,0212446,0},
	{013,0773152,0},
	{024,0274557,0},
	{004,0227474,0},
	{020,0263122,0},
	{004,0333254,0},
	{010,0216672,0},
	{006,0231142,0},
	{006,0212701,0},
	{023,0650051,0},
	{016,0233751,0}
};

// Planet y position from orbit center
double py[32];
const struct flt py_[] = {
	{000,0000000,0},
	{017,0664054,0},
	{002,0662035,0},
	{010,0350757,0},
	{006,0334771,0},
	{000,0267000,0},
	{006,0726770,0},
	{004,0225752,0},
	{006,0201346,0},
	{010,0247536,0},
	{010,0343277,0},
	{011,0314411,0},
	{006,0712237,0},
	{021,0334656,0},
	{017,0342324,0},
	{016,0615151,0},
	{005,0644257,0},
	{005,0622456,0},
	{024,0224063,0},
	{012,0331314,0},
	{006,0640034,0},
	{001,0200024,0},
	{011,0243161,0},
	{023,0772355,0},
	{007,0644432,0},
	{022,0726324,0},
	{006,0260740,0},
	{010,0600213,0},
	{006,0237476,0},
	{005,0674734,0},
	{023,0616334,0},
	{016,0327155,0}
};

double pw[32];

// Planet negative orbital angular velocity squared (i.e. -GM/r^3)
double pww[32];
const struct flt pww_[] = {
	{0000,0000000,0000000},
	{-054,0663265,0376074},
	{-036,0743326,0460356},
	{-043,0647730,0444215},
	{-045,0767246,0341205},
	{-034,0745027,0221674},
	{-036,0702670,0530661},
	{-034,0702340,0273047},
	{-037,0747771,0100452},
	{-041,0743411,0732756},
	{-044,0716603,0200021},
	{-050,0755641,0517072},
	{-035,0751602,0332677},
	{-072,0631066,0300145},
	{-056,0712656,0015171},
	{-050,0701167,0507203},
	{-033,0715124,0202507},
	{-034,0665024,0365605},
	{-073,0730064,0776551},
	{-054,0667362,0431776},
	{-043,0775760,0312631},
	{-030,0740613,0034530},
	{-055,0635657,0370276},
	{-074,0677234,0315321},
	{-037,0617102,0655555},
	{-066,0723370,0773672},
	{-035,0714526,0724272},
	{-043,0667633,0402706},
	{-040,0636053,0440472},
	{-037,0650025,0224325},
	{-071,0717663,0701773},
	{-052,0754612,0304722}
};

double flt2float(struct flt flt)
{
	char a = flt.exp;
	int b = flt.m1;
	int c = flt.m2;
	int s = 1 - 2*(b >> 17);
	b &= 0377777;
	double m = 0;
	int i = 1;
	while (b) {
		m += (double) (b >> 16) / (1<<i);
		b <<= 1;
		b &= 0377777;
		++i;
	}
	i = 18;
	while (c) {
		m += (double) (c >> 17) / (1ll<<i);
		c <<= 1;
		c &= 0777777;
		++i;
	}
	return a > 0 ? s * m * (1ll<<a) : s * m / (1ll<<-a);
}

double rpar, dpar;
double ax, ay, maxa;
int maxj;

double sphi, cphi;
double absx, absy;
double spx, spy;

double shipx, shipy, x, y, ox, oy;
bool lanflg, goflg, forflg, bacflg;

double horizv;

bool inflg, grvflg;
bool crflg;

int par;
double stheta = 1, ctheta = 0;
int scale = 0;

/* Constants */
const int nplan = 32;
const int vscale = 6;
const double ascale = -0.5;
const double fardst = 32768;
const double thrs = 2;
const bool accflg;
bool locflg;
int locpar;
double crash;
double sdphi, cdphi;

int xpos = 0;
int ypos = 0;
int sz = 1;
int br = 1;
bool blnk = false;
bool show = true;

int window_width = 1024;
int window_height = 1024;
double wscale = 1;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

void dsetx(int x)
{
	// ensure planet name is in view
	x -= x < 127 ? min(512, window_width/(2*wscale)) : 512;
	xpos = x;
}

void dsety(int y)
{
	y -= y < 250 ?
		// ensure info is in view
		min(512, window_height/(2*wscale)) :
		// ensure main view is in view
		max(512, 1024 - window_height/(2*wscale));
	ypos = y;
}

void dscale(int s) {
	sz = 1<<s;
}

void intens(enum bri i) {
	br = i+1;
}

void blink(bool s) {
	blnk = s;
}

void vec(int x, int y, bool vis)
{
	if (vis) {
		SDL_SetRenderDrawColor(renderer, 255*br/4, 255*br/4, 255*br/4,
			255);
		SDL_RenderLine(renderer,
			wscale*xpos+window_width/2,
			window_height-(wscale*ypos+window_height/2),
			wscale*(xpos+sz*x)+window_width/2,
			window_height-(wscale*(ypos+sz*y)+window_height/2));
	}
	xpos += sz*x;
	ypos += sz*y;
}

void vecx(int x)
{
	vec(x, 0, true);
}

void vecy(int y)
{
	vec(0, y, true);
}

enum dir { N, NE, E, SE, S, SW, W, NW };

void incr(int n, enum dir dir)
{
	static int xtab[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	static int ytab[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dx = xtab[dir];
	int dy = ytab[dir];
	for (int i = 0; i < n; i++) vec(dx, dy, true);
}

void dchar(char c)
{
	bool vis = false;
	unsigned char *ch = font[c-32];
	unsigned char ij;
	int x = 0, y = 3;
	while ((ij = *ch++) != 0xff) {
		if (ij == 0x0f) vis = false;
		else if (ij == 0x1f) vis = true;
		else {
			int x2 = ij & 0xf;
			int y2 = 0xf - (ij >> 4);
			int dx = x2-x;
			int dy = y2-y;
			vec(dx, dy, vis);
			x += dx;
			y += dy;
		}
	}
	vec(13-x, 3-y, false);
}

void chars(const char *s)
{
	if (blnk && !show) return;
	while (*s) dchar(*s++);
}

int inscr(int a)
{
	a = 383 - a;
	if (a < 0) return 0;
	a -= 768;
	if (a >= 0) return 0;
	return a;
}

void rotx(void)
{
	double s = absx * stheta - absy * ctheta;
	spx = scale > 0 ? s / (1<<scale) : s * (1<<-scale);
}

char sca[4] = " ";

void dssca(void)
{
	chars(sca);
	dscale(0);
	dsetx(127);
	dsety(250);
	vecx(768);
	dsetx(895);
	dsety(255);
	vecy(768);
	dsetx(895);
	dsety(1023);
	vecx(-768);
	dsetx(127);
	dsety(1023);
	vecy(-768);
	dsetx(127);
	dsety(255);
	vecx(768);
	dsetx(511);
	dsety(255);
	vecy(767);
	dsetx(127);
	dsety(639);
	vecx(767);
}

void dspsca(void)
{
	bool neg = scale < 0;
	int s = neg ? -scale : scale;
	sca[0] = neg ? '-' : '+';
	sca[1] = '0' + s / 10;
	sca[2] = '0' + s % 10;
}

void rotate(bool dir)
{
	double ftmp1 = cphi * sdphi;
	if (!dir) ftmp1 = -ftmp1;

	double ftmp2 = sphi * cdphi + ftmp1;

	ftmp1 = sphi * sdphi;
	if (dir) ftmp1 = -ftmp1;

	cphi = cphi * cdphi + ftmp1;
	sphi = ftmp2;
}

bool quit = false;
const bool *pbson;

void contrl(const SDL_Event *e)
{
	if (e) {
		switch (e->type) {
		case SDL_EVENT_KEY_DOWN:
			switch (e->key.key) {
			case SDLK_1:
				quit = true;
				break;
			case SDLK_2:
				goflg = crflg = false;
				break;
			case SDLK_7: case SDLK_DOWN:
				++scale;
				dspsca();
				break;
			case SDLK_8: case SDLK_UP:
				--scale;
				dspsca();
				break;
			}
		}
	}
	forflg = bacflg = false;
	if (pbson[SDL_SCANCODE_3]) {
		forflg = true;
		if (!goflg) lanflg = false;
	}
	if (pbson[SDL_SCANCODE_4]) {
		bacflg = true;
		if (!goflg) lanflg = false;
	}
	if (pbson[SDL_SCANCODE_5] || pbson[SDL_SCANCODE_RIGHT])
		rotate(false);
	if (pbson[SDL_SCANCODE_6] || pbson[SDL_SCANCODE_LEFT])
		rotate(true);
}

void dsplanet(int p)
{
	intens(points[p]);
	incr(1, NE); incr(2, W);
	incr(2, S); incr(2, E);
	incr(1, N); incr(1, W);
}

const char *cl = " ";

void dispcl(void)
{
	chars(cl);
}

void namedsp(void)
{
	chars(names[par]);
}

void displist(void)
{
	dscale(1);
	intens(3);
	blink(true);
	dsetx(800);
	dsety(20);
	dispcl();
	intens(0);
	blink(false);
	dsetx(0);
	dsety(20);
	namedsp();
	dsetx(400);
	dsety(20);
	dssca();
}

/* Update planet position */
void updpln(int p)
{
	// rotate by a (pw[p] = cos(a), pww[p] = sin(a))
	double ftmp1 = px[p];
	px[p] = px[p] * pw[p] - py[p] * pww[p];
	py[p] = ftmp1 * pww[p] + py[p] * pw[p];
}

void invert(int p)
{
	pww[p] = -pww[p];
	updpln(p);
}

/* Get floating-point error of absxy */
void absv(int p)
{
	absx = absy = 0;

	int absi = p;
	while (absi) {
		invert(absi);
		absx -= px[absi];
		absy -= py[absi];

		invert(absi);
		absx += px[absi];
		absy += py[absi];

		absi = ppar[absi];
	}
}

/* Get planet absolute position */
void absxy(int p)
{
	// get distance from planet to sun
	int absi = p;
	absx = 0;
	absy = 0;
	while (absi) {
		absx += px[absi];
		absy += py[absi];
		absi = ppar[absi];
	}
	return;
}

/* Set ship absolute position */
void shipxy(void)
{
	shipx = -(absx + x);
	shipy = -(absy + y);
}

/* Update ship acceleration from planet gravity */
void updacc(int p)
{
	// set absx and absy to distance from ship to planet
	if (p != par) {
		absxy(p);
		absx += shipx;
		absy += shipy;
	} else {
		absx = -x;
		absy = -y;
	}

	double dtmp1 = absy * absy + absx * absx;

	dpar = sqrt(dtmp1);

	if (p == par) {
		double ftmp1 = x - ox;
		double ftmp2 = oy - y;
		horizv = (ftmp2 * x + ftmp1 * y) / dpar;
		// if distance from planet less than radius
		if (dpar < rpar && !lanflg) {
			if (ftmp1 * ftmp1 + ftmp2 * ftmp2 > crash) {
				crflg = goflg = true;
#ifdef __EMSCRIPTEN__
				EM_ASM(Module.oncrash());
#endif
			}
			lanflg = true;
			ftmp1 = rpar / dpar;
			// reset x and y to the edge of the planet
			x *= ftmp1;
			ox = x;
			y *= ftmp1;
			oy = y;
			absxy(par);
			shipxy();
			updacc(par);
		}
	}

	/* upda5 */
	// if the planet is too far, set a flag to not draw its border
	// and ignore its gravity unless it's the sun
	if (dpar > fardst && p) {
		grvflg = true;
		return;
	}
	grvflg = false;

	// a = GM / r^2
	double a = accl[p] / dtmp1;
	if (a > maxa) {
		maxa = a;
		maxj = p;
	}

	// set the new acceleration
	double ftmp1 = a / dpar;
	ax += ftmp1 * absx;
	ay += ftmp1 * absy;
}

/* Draw circle */
bool surf(int nt, int setx, int sety, double wx, double wy, double v, double vv)
{
	int tsetx = -setx;
	dsetx(895+setx);
	int tsety = -sety;
	dsety(1023+sety);
	double twx = wx;
	double twy = wy;
	v = -v;
	for (int i = 0; i < nt; i++) {
		// rotate by a (vv = cos(a), v = sin(a))
		double ftmp2 = vv * twx - v * twy;
		twy = vv * twy + v * twx;
		int res = inscr(twy + spy);
		if (!res) return false;
		int dely = res + tsety;
		tsety -= dely;
		twx = ftmp2;
		res = inscr(twx + spx);
		if (!res) return false;
		int delx = res + tsetx;
		tsetx -= delx;
		vec(delx, dely, true);
	}
	return true;
}

/* Draw planet circle */
void drcirc(int p)
{
	if (grvflg) return;
	double pr = sqrt(prsq[p]);
	double dtmp1 = scale > 0 ? pr / (1<<scale) : pr * (1<<-scale);

	if (dtmp1 < thrs) return;

	dpar = scale > 0 ? dpar / (1<<scale) : dpar * (1<<-scale);
	double dtmp2 = (dpar - dtmp1)/dpar;

	double wy = dtmp2 * spy;
	int res = inscr(wy);
	if (!res) return;
	int sety = res;

	if (!inflg) rotx();

	double wx = dtmp2 * spx;
	res = inscr(wx);
	if (!res) return;
	int setx = res;

	wy -= spy;
	wx -= spx;

	int narcs;
	dtmp1 = dtmp1 * M_PI/10;
	if (dtmp1 > 400) narcs = 400;
	else {
		narcs = dtmp1;
		if (narcs < 20) narcs = 0;
		narcs += 20;
		dtmp1 = narcs;
	}

	// v ~= sin(x) ~= x (at x ~ 0)
	// vv ~= cos(x) = sqrt(1 - sin2(x)) ~= 1 - sin2(x)/2
	// (taylor series expansion sqrt(1 - x^2) = 1 - x^2/2 + ...)
	double v = (2*M_PI) / dtmp1;
	double vv = 1 - (v*v)/2;

	intens(0);
	if (surf(narcs, setx, sety, wx, wy, v, vv)) return;
	surf(narcs, setx, sety, wx, wy, -v, vv);
}

/* Display planet */
void displa(int p)
{
	if (p == locpar) {
		if (locflg) {
			stheta = (sphi * absx + cphi * absy) / dpar;
			ctheta = (cphi * absx - sphi * absy) / dpar;
		} else {
			stheta = sphi;
			ctheta = cphi;
		}
	}
	double f = absy * stheta + absx * ctheta;
	spy = scale > 0 ? f / (1<<scale) : f * (1<<-scale);
	inflg = false;
	int res = inscr(spy);
	if (res) {
		dsety(1023+res);
		rotx();
		inflg = true;
		res = inscr(spx);
		if (res) {
			dsetx(895+res);
			dsplanet(p);
		}
	}
	drcirc(p);
}

/* Update ship thrust acceleration and position */
void updshp(void)
{
	if (forflg || bacflg) {
		double a = forflg && bacflg ? 0 :
			scale > 0 ? ascale * (1<<scale) : ascale / (1<<-scale);
		if (bacflg) a = -a;
		if (forflg && accflg) a += maxa;
		a = -a;
		ax += a * ctheta;
		ay += a * stheta;
	}

	// move ship by current thrust
	double ftmp1 = x*2 - ox + ax;
	ox = x;
	x = ftmp1;
	ftmp1 = y*2 - oy + ay;
	oy = y;
	y = ftmp1;

	// return if we're already set to the planet with strongest gravity
	if (par == maxj) return;

	// update ship absolute position
	absxy(par);
	shipxy();

	// get dx and dy
	absv(par);
	ox = absx + x - ox;
	oy = absy + y - oy;

	// set the current planet to the one with strongest gravity
	par = maxj;

	// get -dx and -dy
	absv(par);
	ox = absx - ox;
	oy = absy - oy;

	// update ship relative position
	absxy(par);
	x = -(absx + shipx);
	ox += x;
	y = -(absy + shipy);
	oy += y;

	// update current planet radius
	rpar = sqrt(prsq[par]);

	dspsca();
}

void loop(void)
{
	// set ship position
	absxy(par);
	shipxy();
	// if not game over
	if (!goflg) {
		ax = ay = maxa = 0;
		/* loop1 */
		// update acceleration and draw planets
		for (int i = 0; i < nplan; i++) {
			updacc(i);
			displa(i);
			if (i) updpln(i);
		}
		/* loop2 */
		// update ship if we haven't landed
		if (!lanflg) updshp();
	}
	/* loop3 */
	int mul = vscale - scale - 1;
	double h = mul > 0 ? horizv * (1<<mul) : horizv / (1<<-mul);
	int res = inscr(h);
	if (res) {
		dsetx(895+res);
		dsety(250);
		dsplanet(0);
	}
	/* loop4 */
	cl = crflg ? "CL" : lanflg ? "L" : " ";
}

void main_loop(void)
{
	SDL_Event e;
	static unsigned t0 = 0, t;

	t = SDL_GetTicks();

#ifdef __EMSCRIPTEN__
	if (t - t0 < 1000/60) return;
#endif

	if (!quit) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) quit = true;
			else if (e.type == SDL_EVENT_WINDOW_RESIZED) {
				window_width = e.window.data1;
				window_height = e.window.data2;
				wscale = max(
					min(window_width, window_height), 1024
				) / 1024.0;
			}
			else contrl(&e);
			return;
		}
		contrl(NULL);
		if (t/1000 > t0/1000) show = !show;
		t0 = t;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		displist();
		loop();
		SDL_RenderPresent(renderer);
		return;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();
#else
	exit(EXIT_SUCCESS);
#endif
}

int main(void)
{
	crash = flt2float((struct flt){-027,0200000,0});
	sdphi = sin(1.2 * M_PI/180);
	cdphi = cos(1.2 * M_PI/180);

	for (int i = 0; i < nplan; i++) {
		prsq[i] = flt2float(prsq_[i]);
		accl[i] = flt2float(accl_[i]);
		px[i] = flt2float(px_[i]);
		py[i] = flt2float(py_[i]);
		pww[i] = flt2float(pww_[i]);
		pw[i] = 1 - (pww[i] * pww[i])/2;
	}

	lanflg = true;
	crflg = goflg = forflg = bacflg = locflg = false;
	locpar = 0;

	// start on planet earth
	par = 1;

	// set initial position and orientation
	oy = y = rpar = sqrt(prsq[par]);
	stheta = sphi = 1;
	ctheta = cphi = ox = x = 0;

	dspsca();

	if (!SDL_SetAppMetadata("Space Travel", "1.0.0", "am.akr.st")) {
		fprintf(stderr, "Failed to set app metadata: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialize SDL: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer(
		"Space Travel", window_width, window_height,
		SDL_WINDOW_RESIZABLE, &window, &renderer
	)) {
		fprintf(stderr, "Failed to create window: %s\n",
			SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_SetWindowMinimumSize(window, 848, 848);

	const SDL_DisplayMode *dm =
		SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
	int size = min(min(dm->w, dm->h)-128, 1024);
	window_width = window_height = size;
	SDL_SetWindowSize(window, window_width, window_height);

	pbson = SDL_GetKeyboardState(NULL);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (true) {
		main_loop();
		SDL_Delay(1000/60);
	}
#endif

	return EXIT_SUCCESS;
}
