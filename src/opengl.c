#include <stdio.h>
#include <stdlib.h>

#include <GL/glx.h>    /* this includes the necessary X headers */
#include <GL/gl.h>

#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>

#include "../headers/wavelib.h"
#include "../headers/addfun.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"

static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

Display   *dpy;
Window     win;
GLfloat    xAngle = 42.0, yAngle = 82.0, zAngle = 112.0;
GLboolean  doubleBuffer = GL_TRUE;

int h_window_map = 1024;
int w_window_map = 1024;
int w_window_panel = 512;

void fatalError(char *message)
{
  fprintf(stderr, "main: %s\n", message);
  exit(1);
}

uint32_t symbols_id[256];
int n_symbols = 62;


typedef struct img_size {
	int w;
	int h;
} img_size;


img_size g_symbol_sizes[256];
int g_width, g_height, n_chanels;
void *g_data;

void init_text() {
	glGenTextures(256, symbols_id);
	int start = 0x80;
	char name[30];
	for (int i = 0; i < n_symbols; ++i) {
		if (start == 0xB0)
			start = 0xE0;
		sprintf(name, "./images/%hhx.png", start);
		printf("NAME: %s\n", name);
		g_data = stbi_load(name, &g_symbol_sizes[start].w, &g_symbol_sizes[start].h, &n_chanels, 0);
		printf("%s\n", name);
		if (g_data) {
			glBindTexture(GL_TEXTURE_2D, symbols_id[start]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_symbol_sizes[start].w, g_symbol_sizes[start].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, g_data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else {
			printf("FAIL\n");
		}
		++start;
	}
	start = 0x20;
	for (int i = 0; i < 26; ++i) {
		sprintf(name, "./images/%hhx.png", start);
		printf("NAME: %s\n", name);
		g_data = stbi_load(name, &g_symbol_sizes[start].w, &g_symbol_sizes[start].h, &n_chanels, 0);
		printf("%s\n", name);
		if (g_data) {
			glBindTexture(GL_TEXTURE_2D, symbols_id[start]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_symbol_sizes[start].w, g_symbol_sizes[start].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, g_data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else {
			printf("FAIL\n");
		}
		++start;
	}
}

void draw_text(char *text, vec4 text_color, int start_x, int start_y, float k) {
	float current_x = start_x;
	float current_y = start_y;
	int code;
	for (char *s = text; *s != '\0'; ++s) {
		if (*s < 0)
			code = 256 + *s;
		else
			code = *s;
		// printf("%c\n", *s);
		// printf("%d\n", *s);
		// printf("%d\n", 254 + *s);
		// printf("%d\n", code);
		glBindTexture(GL_TEXTURE_2D, symbols_id[code]); 
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		if (code == 0xE0 || code == 0xE9 || code == 0xE3)
			current_y -= 8;
		glTranslatef(current_x, current_y, 0);

			glBegin(GL_TRIANGLE_FAN);
			glColor4f(text_color.x, text_color.y, text_color.z, text_color.a);
				glTexCoord2d(0, 0);     // Texture coords for lower left corner
				glVertex3f(0, 0, -4);
				glTexCoord2d(1, 0);     // Texture coords for lower right corner
				glVertex3f((float)g_symbol_sizes[code].w * k, 0, -4);
				glTexCoord2d(1, 1);     // Texture coords for upper right corner
				glVertex3f((float)g_symbol_sizes[code].w * k, (float)g_symbol_sizes[code].h * k, -4);     // Texture coords for upper right corner
				glTexCoord2d(0, 1);     // Texture coords for upper left corner
				glVertex3f(0, (float)g_symbol_sizes[code].h * k, -4);
			glEnd();
			// printf("INFO %d %d\n", g_symbol_sizes[code].w, g_symbol_sizes[code].h);


    	glPopMatrix();

		current_x += g_symbol_sizes[code].w * k;
		// printf("Curren_y = %f\n", current_y);
		if (code == 0xE0 || code == 0xE9 || code == 0xE3)
			current_y += 8;
		glDisable(GL_TEXTURE_2D);
	}
}

void motion(int x, int y) {
    y = 1024 - y;

    unsigned char pixel[4];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
	printf("R: %d\n", (int)pixel[0]);
	printf("G: %d\n", (int)pixel[1]);
	printf("B: %d\n", (int)pixel[2]);
    // cout << "R: " << (int)pixel[0] << endl;
    // cout << "G: " << (int)pixel[1] << endl;
    // cout << "B: " << (int)pixel[2] << endl;
    // cout << endl;
}

void get_metrics(map_object *map, int *original_matrix_color, int *original_matrix, metrics *result) {
    unsigned char pixel[4];
	double res_MSE = 0;
	double res_MAE = 0;
	double res_MaAE = 0;
	double res_MaAEC = 0;
	double res_HR = 0;
	double res_HRC = 0;
	int coef = 1 << (map->max_level - map->cur_level);
	printf("Coef = %d\n", coef);
	int i1, j1;
	double res_1 = 0;
	int i2, j2;
	double res_2 = 0;
	for (int i = 0; i < map->len_column; ++i) {
		for (int j = 0; j < map->len_row; ++j) {
			glReadPixels(i, j, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
			res_MSE += (pixel[0] - original_matrix_color[(i) * map->len_row + j]) * (pixel[0] - original_matrix_color[(i) * map->len_row + j]); 
			res_MAE += abs(pixel[0] - original_matrix_color[(i) * map->len_row + j]);
			res_MaAE = fmax(res_MaAE, abs(map->matrix[(i / coef ) * map->len_row + j / coef] - original_matrix[i * map->len_row + j]));
			if (res_1 != res_MaAE) {
				i1 = i;
				j1 = j;
				printf("cur_mat = %f; OM = %d\n", map->matrix[(i / coef ) * map->len_row + j / coef], original_matrix[i * map->len_row + j]);
				res_1 = res_MaAE;
			}
			res_MaAEC = fmax(res_MaAEC, abs(pixel[0] - original_matrix_color[i * map->len_row + j]));
			if (res_2 != res_MaAEC) {
				i2 = i;
				j2 = j;
				printf("pixel = %d; OMC = %d\n", pixel[0], original_matrix_color[(i) * map->len_row + j]);
				res_2 = res_MaAEC;
			}
			if (map->matrix[(i / coef ) * map->len_row + j / coef] - original_matrix[i * map->len_row + j] < 0) {
				res_HR = - map->matrix[(i / coef ) * map->len_row + j / coef] + original_matrix[i * map->len_row + j];
			}
			if (pixel[0] - original_matrix_color[(i) * map->len_row + j] < 0) {
				res_HRC = - pixel[0] + original_matrix_color[(i) * map->len_row + j];
			}
			// printf("Pix = %d; OMC = %d, res = %lf\n", pixel[0], original_matrix_color[i * map->len_row + j], res_MaAEC);
		}
	}
	printf("max_h = %d\n", map->max_h);
	printf("i1 = %d; j1 = %d\n", i1, j1);
	printf("i2 = %d; j2 = %d\n", i2, j2);
	result->MSE = res_MSE / (map->len_column * map->len_row);
	result->MAE = res_MAE / (map->len_column * map->len_row);
	result->MaAE = res_MaAE;
	result->MaAEC = res_MaAEC;
	result->HR = res_HR;
	result->HRC = res_HRC;
}

void redraw(void) {
	static GLboolean   displayListInited = GL_FALSE;
	
	if (displayListInited) {
	  /* if display list already exists, just execute it */
	  glCallList(1);
	}
	else {
	  /* otherwise compile and execute to create the display list */
	  glNewList(1, GL_COMPILE_AND_EXECUTE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	  /* front face */
	  glBegin(GL_QUADS);
	    glColor3f(0.0, 0.7, 0.1);  /* green */
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	
	    /* back face */
	    glColor3f(0.9, 1.0, 0.0);  /* yellow */
	    glVertex3f(-1.0, 1.0, -1.0);
	    glVertex3f(1.0, 1.0, -1.0);
	    glVertex3f(1.0, -1.0, -1.0);
	    glVertex3f(-1.0, -1.0, -1.0);
	
	    /* top side face */
	    glColor3f(0.2, 0.2, 1.0);  /* blue */
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, -1.0);
	    glVertex3f(-1.0, 1.0, -1.0);
	
	    /* bottom side face */
	    glColor3f(0.7, 0.0, 0.1);  /* red */
	    glVertex3f(-1.0, -1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(1.0, -1.0, -1.0);
	    glVertex3f(-1.0, -1.0, -1.0);
	  glEnd();
	  glEndList();
	  displayListInited = GL_TRUE;
	}
	if (doubleBuffer)
	  glXSwapBuffers(dpy, win);/* buffer swap does implicit glFlush */
	else
	  glFlush();  /* explicit flush for single buffered case */
}

void draw_display(map_object *map) {
	glLoadIdentity();
	glViewport(w_window_map, 0, w_window_map, h_window_map);
	glClearColor(0.0, 0.0, 0.7, 0.0);
	int x = 20;
	int y = 900;
	int w = 472;
	int h = 100;
	glColor4f(0.f, 0.f, 0.f, 1.f);
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, -5);
		glVertex3f(x + w, y, -5);
		glVertex3f(x + w, y - h, -5);
		glVertex3f(x, y - h, -5);
	glEnd();

	y -= 200;
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, -5);
		glVertex3f(x + w, y, -5);
		glVertex3f(x + w, y - h, -5);
		glVertex3f(x, y - h, -5);
	glEnd();

	// Button '+'
	x = 206;
	y = 405;
	w = h;
	glColor4f(0.5, 0.5, 0.5, 1.f);
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, -5);
		glVertex3f(x + w, y, -5);
		glVertex3f(x + w, y - h, -5);
		glVertex3f(x, y - h, -5);
	glEnd();


	// Button '+'
	x = 206;
	y = 200;
	w = h;
	glColor4f(0.5, 0.5, 0.5, 1.f);
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, -5);
		glVertex3f(x + w, y, -5);
		glVertex3f(x + w, y - h, -5);
		glVertex3f(x, y - h, -5);
	glEnd();

	char *text = "\x8c\xa0\xaa\xe1\xa8\xac\xa0\xab\xec\xad\xeb\xa9\x20\xe3\xe0\xae\xa2\xa5\xad\xec"; 
	vec4 red_color = {1.f, 0.f, 0.f, 1.f};
	vec4 white_color = {1.f, 1.f, 1.f, 1.f};
	draw_text(text, red_color, 20, 905, 0.4);
	char number[5];// = "\x31\x30";
	sprintf(number, "%d", map->max_level);
	draw_text(number, white_color, 20, 805, 1);
	text = "\x92\xa5\xaa\xe3\xe9\xa8\xa9\x20\xe3\xe0\xae\xa2\xa5\xad\xec";
	draw_text(text, red_color, 20, 705, 0.4);
	sprintf(number, "%d", map->cur_level);
	draw_text(number, white_color, 20, 605, 1);
	sprintf(number, "%c", '+');
	// text = "\x8d\xa0\xa6\xac\xa8\xe2\xa5\x20\x2b\x20\xe7\xe2\xae\xa1\xeb\x20\xe3\xa2\xa5\xab\xa8\xe7\xa8\xe2\xec\x20\xe3\xe0\xae\xa2\xa5\xad\xec\x20\xa4\xa5\xe2\xa0\xab\xa8\xa7\xa0\xe6\xa8\xa8";
	draw_text(number, white_color, 233, 335, 1);
	sprintf(number, "%c", '-');
	draw_text(number, white_color, 233, 128, 1);
}


void draw(map_object *map) {
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, w_window_map, h_window_map);
	float r, g, b;
	int shift_i = 1 << (map->max_level - map->cur_level);
	int shift_j = 1 << (map->max_level - map->cur_level);
	// printf("shift_i = %d\n", shift_i);
	// printf("len_column = %d\n", map->len_column);
	// printf("len_row = %d\n", map->len_row);
	int i, j, l_i, l_j;
	l_i = 0; l_j = 0;
	int new_pos_j;

	glBegin(GL_POINTS);

	for (i = 0; i < map->len_column; i += shift_i) {
		for (j = 0; j < map->len_row; j += shift_j) {
				r =	(float)map->matrix[(i / shift_i) * map->len_row + j / shift_j] / map->max_h;
				g = (float)map->matrix[(i / shift_i) * map->len_row + j / shift_j] / map->max_h;
				b = (float)map->matrix[(i / shift_i) * map->len_row + j / shift_j] / map->max_h;
				glColor4f(r, g, b,  1.f);
				for (l_i = 0; l_i < shift_i; ++l_i) {
					for (l_j = 0; l_j < shift_j; ++l_j) {
				 		glVertex3f(j + l_j, map->len_row - (i + l_i) -1, -100);
					}
				}
		}
	}

	glEnd();

	draw_display(map);

	if (doubleBuffer)
	  glXSwapBuffers(dpy, win);/* buffer swap does implicit glFlush */
	else
	  glFlush();  /* explicit flush for single buffered case */
} 

int run(int argc, char **argv, map_object *map) {
	XVisualInfo         *vi;
	Colormap             cmap;
	XSetWindowAttributes swa;
	GLXContext           cx;
	XEvent               event;
	GLboolean            needRedraw = GL_FALSE, recalcModelView = GL_TRUE;
	int                  dummy;
	
	/*** (1) open a connection to the X server ***/
	
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	  fatalError("could not open display");
	
	/*** (2) make sure OpenGL's GLX extension supported ***/
	
	if(!glXQueryExtension(dpy, &dummy, &dummy))
	  fatalError("X server has no OpenGL GLX extension");
	
	/*** (3) find an appropriate visual ***/
	
	/* find an OpenGL-capable RGB visual with depth buffer */
	vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	if (vi == NULL)
	{
	  vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
	  if (vi == NULL) fatalError("no RGB visual with depth buffer");
	  doubleBuffer = GL_FALSE;
	}
	if(vi->class != TrueColor)
	  fatalError("TrueColor visual required for this program");
	
	/*** (4) create an OpenGL rendering context  ***/
	
	/* create an OpenGL rendering context */
	cx = glXCreateContext(dpy, vi, /* no shared dlists */ None,
	                      /* direct rendering if possible */ GL_TRUE);
	if (cx == NULL)
	  fatalError("could not create rendering context");
	
	/*** (5) create an X window with the selected visual ***/
	
	/* create an X colormap since probably not using default visual */
	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = KeyPressMask    | ExposureMask
	               | ButtonPressMask | StructureNotifyMask;
	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0,
	                    w_window_map + w_window_panel, h_window_map, 0, vi->depth, InputOutput, vi->visual,
	                    CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, "main", "main", None,
	                       argv, argc, NULL);
	
	/*** (6) bind the rendering context to the window ***/
	
	glXMakeCurrent(dpy, win, cx);
	
	/*** (7) request the X window to be displayed on the screen ***/
	
	XMapWindow(dpy, win);
	
	/*** (8) configure the OpenGL context for rendering ***/
	
	glEnable(GL_DEPTH_TEST); /* enable depth buffering */
	glDepthFunc(GL_LESS);    /* pedantic, GL_LESS is the default */
	glClearDepth(1.0);       /* pedantic, 1.0 is the default */
	
	/* frame buffer clears should be to black */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	/* set up projection transform */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
	/* establish initial viewport */
	glOrtho(0.f, 1024.f, 0.f, 1024.f, 1.0, 600.0);
	// glOrtho(0.f, 2048.f, 0.f, 2048.f, 1.0, 600.0);
	/* pedantic, full window size is default viewport */
	// glViewport(0, 0, 300, 300);
	glViewport(0, 0, 1024, 1024);

	
	printf( "Press left mouse button to rotate around X axis\n" );
	printf( "Press middle mouse button to rotate around Y axis\n" );
	printf( "Press right mouse button to rotate around Z axis\n" );
	printf( "Press ESC to quit the application\n" );
	
	/*** (9) dispatch X events ***/
	
	init_text();

	int *original_matrix_color = (int *)malloc(sizeof(int) * map->len_row * map->len_column);
	int *original_matrix = (int *)malloc(sizeof(int) * map->len_row * map->len_column);
	metrics results;

	char init_matrix = 0;

	while (1) {
		do {
			XNextEvent(dpy, &event);
			switch (event.type) {
				case KeyPress: {
					KeySym     keysym;
				  	XKeyEvent *kevent;
				  	char       buffer[1];
					/* It is necessary to convert the keycode to a
				  	 * keysym before checking if it is an escape */
					kevent = (XKeyEvent *) &event;
					recalcModelView = GL_TRUE;
					if (   (XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL) == 1)
				  	    && (keysym == (KeySym)XK_Escape) )
				  	  exit(0);
				  	else if (keysym == (KeySym)XK_Up) {
				  	    haar_2D_reconstruction(map, 1);
				  	    print_map_info(map);
				  	}
				  	else if (keysym == (KeySym)XK_Down) {
				  	    haar_2D_decomposition(map, 1);
				  	    print_map_info(map);
				  	}
					else if (keysym == (KeySym)XK_g) {
						get_metrics(map, original_matrix_color, original_matrix, &results);
						printf("MSE = %lf\n", results.MSE);
						printf("MAE = %lf\n", results.MAE);
						printf("MaAE = %lf\n", results.MaAE);
						printf("MaAEC = %lf\n", results.MaAEC);
						printf("HR = %lf\n", results.HR);
						printf("HRC = %lf\n", results.HRC);
						printf("\n\n\n");
					}
				  	break;
				}
				case ButtonPress:
					// recalcModelView = GL_TRUE;
					printf("COORDS %d %d\n", event.xbutton.x, event.xbutton.y);
					motion(event.xbutton.x, event.xbutton.y);
					// printf("MSE = %f\n", find_MSE(map, original_matrix));
					if (event.xbutton.x >= 1200 && event.xbutton.x <= 1330) {
						if (event.xbutton.y >= 610 && event.xbutton.y <= 730) {
							haar_2D_reconstruction(map, 1);
						}
						if (event.xbutton.y >= 800 && event.xbutton.y <= 920) {
							haar_2D_decomposition(map, 1);
						}
					}
				case ConfigureNotify:
				  // glViewport(0, 0, event.xconfigure.width,
				  //            event.xconfigure.height);
				  /* fall through... */
				case Expose:
					needRedraw = GL_TRUE;
					break;
			}
		} while(XPending(dpy)); /* loop to compress events */
		if (recalcModelView) {
			glMatrixMode(GL_MODELVIEW);
			
			/* reset modelview matrix to the identity matrix */
			glLoadIdentity();
			
			/* move the camera back three units */
			glTranslatef(0.0, 0.0, -3.0);

			recalcModelView = GL_FALSE;
			needRedraw = GL_TRUE;
		}
		if (needRedraw) {
			draw(map);
			needRedraw = GL_FALSE;
			++init_matrix;
		}
		if (init_matrix == 2) {
			unsigned char pixel[4];
			printf("HERE--------------\n");
			for (int i = 0; i < map->len_column; ++i) {
				for (int j = 0; j < map->len_row; ++j) {
					glReadPixels(i, j, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
					original_matrix_color[i * map->len_row + j] = pixel[0];
					original_matrix[i * map->len_row + j] = map->matrix[i * map->len_row + j];
				}
			}
			// init_matrix = 1;
		}
	}

	return 0;
}
