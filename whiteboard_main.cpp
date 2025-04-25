#include "iGraphics.h"
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#define screenwidth 1200
#define screenheight 600

enum choice {INACTIVE,PEN, CURSOR, TEXT, SHAPE,ERASER};
enum choice chc = PEN;

//info about files
int id_of_file;
int no_of_files;
char file_names[10][40];

int current_board=0; //for controlling multiple boards
char board[7]; // displaying board no in bottom
bool is_dark_mode=0; //for controlling dark mode


int current_size=1;

int point_indexes[10]={0}; //no. of points for 10 diff boards
int text_index[10]={0}; //No. of text for 10 diff boards
int rectangle_index[10]={0}, circle_index[10]={0}, line_index[10]={0};
int eraser_index[10] = {0};
int current_layer[10]={0};

int color_r=0,color_g=0,color_b=0; //this indicates current color, initially set to black

int n_place=1; //for drawing minimal size points
int text_char_id=0; // when taking text input this indicates index of character


int menu_height = 50;
int menu_width = 2*screenwidth/5;
int menu_x = screenwidth/2-screenwidth/5;
int menu_y=screenheight-menu_height;

int gap_x=15;
const int common_width=30;
const int common_height=30;

//left menu
//save
int save_icn_x = 5;
int lm_y = screenheight-common_width-5;

//load
int load_icn_x = save_icn_x +common_width+ gap_x;
bool is_load_clicked=0;
int load_window_dx = 400;
int load_window_dy;
int load_window_x = screenwidth/2 - load_window_dx/2;
int load_window_y;

//right menu vars
int arr_dim=20;
int rm_x=screenwidth-70; //right menu which includes only two arrows
int rm_y = screenheight-40;
int left_arrow_x=rm_x+5;
int right_arrow_x = left_arrow_x+arr_dim+10;
int arrow_y=rm_y+5;

//middle menu
int middle_menu_itm_y = menu_y+10; //all y coordinate of the items is equal

bool color_clicked =0;
bool is_color_menu=0;
int color_circle_rad=gap_x;
int color_icn_x=menu_x+gap_x+color_circle_rad+5;
int color_icn_y = menu_y+menu_height/2;

int pen_x = color_icn_x+color_circle_rad+gap_x;

int pointer_x=pen_x + common_width + gap_x;

bool text_active=0;
int text_icn_x = pointer_x + common_width + gap_x;

int size_icn_x = text_icn_x +common_width+gap_x;
bool is_size_menu=0;

int eraser_icn_x = size_icn_x+common_width+gap_x;

int shapes_icn_x = eraser_icn_x+common_width+gap_x;
bool is_shapes_menu = 0;
bool is_draw_line=0;
bool is_draw_rectangle=0;
bool is_draw_square=0;
bool is_draw_circle=0;
bool is_draw_uniform_circle=0;

int gridlines_icn_x=shapes_icn_x+common_width+gap_x;
bool show_gridlines=0;

int CFB_x = gridlines_icn_x+common_width+gap_x; //CFB = clear full board

int dark_mode_x = CFB_x + gap_x+common_width;

//structure definitions
struct LAYER_SEPARATION_{
	int p_sep, l_sep, r_sep, c_sep,t_sep,e_sep;
};

struct POINT_{
	int x1=0,x2=0;
	int y1=0,y2=0;
	int r,g,b;
	int size;
};

struct TEXT_{
	int x,y;
	char text[200];
	int r,g,b;
	int size;
};

struct RECTANGLE_{
	int x,dx,y,dy;
	int r,g,b;
	bool is_filled=0;
};

struct CIRCLE_{
	int x,y,r1,r2; //r1 = a of ellipse, r2 = b of ellipse, in case of circle r1=r2
	int r,g,b;
	bool is_filled =0;
};

struct LINE_{
	int x1,x2,y1,y2;
	int r,g,b;
};

struct ERASE_P_{
	int x,y;
};

//structure declarations
struct LAYER_SEPARATION_ layer_seps[10][100];
struct TEXT_ texts[10][100];
struct POINT_ points[10][screenwidth*screenheight];
struct ERASE_P_ erased_points[10][screenwidth*screenheight/100];
struct LINE_ lines[10][100];
struct RECTANGLE_ rectangles[10][50];
struct CIRCLE_ circles [10][50];

HCURSOR hcursor;
FILE *doc_file, *save_load_file;

//function protoypes
void initBoard();
void updateFileInfo(void);
void updateLayerInfo(void);
void drawColorMenu(void);
void drawMainMenu(void);
void drawUpperArrows(void);
void drawSaveLoad(void);
void drawSizeMenu(void);
void drawShapesMenu(void);
void drawCurrentShape(void);
void drawLines(int st,int end);
void drawRectangles(int st, int end);
void drawCircles(int st, int end);
void erase_point(struct POINT_ *p);
void erase_text(struct TEXT_ *t);
void erase_line(struct LINE_ *l);
void erase_rectangle(struct RECTANGLE_ *r);
void erase_circle(struct CIRCLE_ *c);
void erase_erasers(struct ERASE_P_ *e);
void erase_layers(struct LAYER_SEPARATION_ *ls);
void save_current_file();
void show_load_window();
int get_load_index(int my);
void load_file(int index);
void clear_full_board(int id);

//initialize board with necessary info, i.e: board_id, file names etc
void initBoard(){

	if((doc_file=fopen("Saved_files\\info","r"))==NULL){
		printf("Error opening file on read!");
		exit(-5);
	}
	fscanf(doc_file,"boards: %d\n",&no_of_files);
	for(int i=0; i<no_of_files;i++){
		fscanf(doc_file,"%s",file_names[i]);
		strcat(file_names[i],"");
	}
	id_of_file=no_of_files;
	fclose(doc_file);

}

//update info file with new file name and increasing board numbers
void updateFileInfo(void){

	if((doc_file=fopen("Saved_files\\info","w"))==NULL){
		printf("Error opening file.");
		exit(-5);
	}
	fprintf(doc_file,"boards: %d\n",no_of_files);
	for(int i=0; i<no_of_files;i++){
		fprintf(doc_file,"%s\n",file_names[i]);
	}
	fclose(doc_file);

}

//updating info about current layer. This function will be called continuously
void updateLayerInfo(void){
	layer_seps[current_board][current_layer[current_board]].p_sep = point_indexes[current_board];
	layer_seps[current_board][current_layer[current_board]].l_sep = line_index[current_board];
	layer_seps[current_board][current_layer[current_board]].r_sep = rectangle_index[current_board];
	layer_seps[current_board][current_layer[current_board]].c_sep = circle_index[current_board];
	layer_seps[current_board][current_layer[current_board]].t_sep = text_index[current_board];
	layer_seps[current_board][current_layer[current_board]].e_sep = eraser_index[current_board];
}

void iDraw() {

	updateLayerInfo();
	iClear(); 
	//maintaining cursor (OPTIONAL)
	if(chc==PEN){
		hcursor = LoadCursor(NULL,IDC_CROSS);
		SetCursor(hcursor);
	}
	else if(chc==TEXT){
		hcursor = LoadCursor(NULL, IDC_IBEAM);
		SetCursor(hcursor);
	}
	else if(chc==ERASER){
		hcursor = LoadCursor(NULL,IDC_CROSS);
		SetCursor(hcursor);
	}
	
	if(is_dark_mode) iSetColor(20,20,20); //background color for dark mode
	else	iSetColor(255,255,255);
	iFilledRectangle(0,0,screenwidth,screenheight); //the background board

	//showing gridlines, if it is selected
	if(show_gridlines){
		int j;
		for(j=0;j<screenwidth;j+=50){
			if(j%250==0) is_dark_mode? iSetColor(255-110,255-110,255-110): iSetColor(110,110,110);
			else is_dark_mode? iSetColor(255-200,255-200,255-200): iSetColor(200,200,200);
			iLine(j,0,j,screenheight);
		}
		for(j=0;j<screenheight;j+=50){
			if(j%250==0) is_dark_mode? iSetColor(255-110,255-110,255-110): iSetColor(110,110,110);
			else is_dark_mode?iSetColor(255-200,255-200,255-200): iSetColor(200,200,200);
			iLine(0,j,screenwidth,j);
		}
	}

	//this variables are used to control layer and eraser
	int id_p=0, id_r=0, id_c=0, id_l=0, id_t=0,id_e=0;

	for(int a=0; a<=current_layer[current_board];a++){
		
		while(id_p<layer_seps[current_board][a].p_sep){
			if(is_dark_mode && points[current_board][id_p].r==0&& points[current_board][id_p].g==0 && points[current_board][id_p].b==0)
				iSetColor(255-points[current_board][id_p].r,255-points[current_board][id_p].g,255-points[current_board][id_p].b);

			else
				iSetColor(points[current_board][id_p].r,points[current_board][id_p].g,points[current_board][id_p].b);
			
			if(points[current_board][id_p].size==1)
				iLine(points[current_board][id_p].x1,points[current_board][id_p].y1,points[current_board][id_p].x2,points[current_board][id_p].y2);
			else{
				iPoint(points[current_board][id_p].x1,points[current_board][id_p].y1,points[current_board][id_p].size==2? 6:12);
			}
			id_p++;
		}

		while(id_t<=layer_seps[current_board][a].t_sep){
			if(is_dark_mode&& texts[current_board][id_t].r==0&& texts[current_board][id_t].g==0 && texts[current_board][id_t].b==0) iSetColor(255-texts[current_board][id_t].r,255-texts[current_board][id_t].g,255-texts[current_board][id_t].b);
			else 	iSetColor(texts[current_board][id_t].r,texts[current_board][id_t].g,texts[current_board][id_t].b);
			if(texts[current_board][id_t].size==1) 	iText(texts[current_board][id_t].x, texts[current_board][id_t].y,texts[current_board][id_t].text,GLUT_BITMAP_HELVETICA_12);
			else iText(texts[current_board][id_t].x, texts[current_board][id_t].y,texts[current_board][id_t].text,GLUT_BITMAP_TIMES_ROMAN_24);
			id_t++;
		}
		id_t--;

		drawLines(id_l,layer_seps[current_board][a].l_sep);
		id_l = layer_seps[current_board][a].l_sep;
		drawRectangles(id_r,layer_seps[current_board][a].r_sep);
		id_r = layer_seps[current_board][a].r_sep;
		drawCircles(id_c,layer_seps[current_board][a].c_sep);
		id_c = layer_seps[current_board][a].c_sep;

		while(id_e<layer_seps[current_board][a].e_sep){
			if(is_dark_mode)
				iSetColor(20,20,20);
			else
				iSetColor(255,255,255);
			iPoint(erased_points[current_board][id_e].x,erased_points[current_board][id_e].y,15);
			id_e++;
		}
	}

	drawSaveLoad(); //drawing top-left menu
	drawUpperArrows(); //drawing top-right menu

	is_dark_mode?iSetColor(255,255,255):iSetColor(0,0,0);
	sprintf(board, "%d/10",current_board+1); //for board no at left bottom corner
	iText(10,10,board,GLUT_BITMAP_HELVETICA_12); //drawing board no at bottom

	drawMainMenu();	
	if(is_color_menu)	drawColorMenu();
	if(is_size_menu)	drawSizeMenu();
	if(is_shapes_menu)		drawShapesMenu();
	if(is_load_clicked)	show_load_window();
	if(is_draw_circle||is_draw_line||is_draw_rectangle) drawCurrentShape();
	
	
}

void drawLines(int st,int end){
	for(int i=st; i<=end;i++){
		if(is_dark_mode&& lines[current_board][i].r==0 &&lines[current_board][i].g==0 && lines[current_board][i].b==0) iSetColor(255-lines[current_board][i].r,255-lines[current_board][i].g,255-lines[current_board][i].b);
		else	iSetColor(lines[current_board][i].r,lines[current_board][i].g,lines[current_board][i].b);
		iLine(lines[current_board][i].x1,lines[current_board][i].y1,lines[current_board][i].x2,lines[current_board][i].y2);
	}
}

void drawRectangles(int st, int end){
	for(int i=st; i<=end;i++){
		if(is_dark_mode&& rectangles[current_board][i].r==0 &&rectangles[current_board][i].g==0 && rectangles[current_board][i].b==0) iSetColor(255-rectangles[current_board][i].r,255-rectangles[current_board][i].g,255-rectangles[current_board][i].b);
		else	iSetColor(rectangles[current_board][i].r,rectangles[current_board][i].g,rectangles[current_board][i].b);
		if(rectangles[current_board][i].is_filled)
			iFilledRectangle(rectangles[current_board][i].x,rectangles[current_board][i].y,rectangles[current_board][i].dx,rectangles[current_board][i].dy);
		else
			iRectangle(rectangles[current_board][i].x,rectangles[current_board][i].y,rectangles[current_board][i].dx,rectangles[current_board][i].dy);
	}
}

void drawCircles(int st, int end){
	for(int i=st; i<=end;i++){
		if(is_dark_mode&& circles[current_board][i].r==0 &&circles[current_board][i].g==0 && circles[current_board][i].b==0) iSetColor(255-circles[current_board][i].r,255-circles[current_board][i].g,255-circles[current_board][i].b);
		else	iSetColor(circles[current_board][i].r,circles[current_board][i].g,circles[current_board][i].b);
		if(circles[current_board][i].is_filled)
			iFilledEllipse(circles[current_board][i].x,circles[current_board][i].y,circles[current_board][i].r1,circles[current_board][i].r2);
		else
			iEllipse(circles[current_board][i].x,circles[current_board][i].y,circles[current_board][i].r1,circles[current_board][i].r2);
	}
}

void drawMainMenu(void){
	is_dark_mode? iSetColor(60,60,60) : iSetColor(250,250,250);
	iFilledRectangle(menu_x,menu_y,menu_width,menu_height); //drawing menu canvas at top-middle
	if(!is_dark_mode) iSetColor(60,60,60);
	iRectangle(menu_x-2,menu_y-2,menu_width+4,menu_height+4);
	if(is_dark_mode && color_r==0 && color_b==0 && color_g==0)	iSetColor(255-color_r,255-color_g,255-color_b);	
	else	iSetColor(color_r,color_g,color_b);
	iFilledCircle(color_icn_x,color_icn_y,color_circle_rad);
	iShowBMP(pen_x,middle_menu_itm_y,"Images\\pen.bmp");
	if(is_dark_mode &&color_r==0 && color_b==0 && color_g==0 ) iSetColor(255-color_r,255-color_g,255-color_b);
	else	iSetColor(color_r,color_g,color_b);
	iFilledCircle(color_icn_x,color_icn_y,color_circle_rad,1000);
	iShowBMP(pointer_x,middle_menu_itm_y,"Images\\cursor_icn.bmp");
	iShowBMP(text_icn_x,middle_menu_itm_y,"Images\\text.bmp");
	iShowBMP(size_icn_x,middle_menu_itm_y,"Images\\size_icn.bmp");
	iShowBMP(eraser_icn_x,middle_menu_itm_y,"Images\\eraser.bmp"); //only this left
	iShowBMP(shapes_icn_x,middle_menu_itm_y,"Images\\shapes_icn.bmp");	
	iShowBMP(gridlines_icn_x,middle_menu_itm_y,"Images\\gridlines_icn.bmp");
	iShowBMP(CFB_x,middle_menu_itm_y,"Images\\CFB.bmp"); 
	if(is_dark_mode) iShowBMP(dark_mode_x,middle_menu_itm_y,"Images\\light_mode_icn.bmp");
	else iShowBMP(dark_mode_x,middle_menu_itm_y,"Images\\dark_mode_icn.bmp");
	
	iSetColor(0,0,0);
	if(chc==TEXT) iRectangle(text_icn_x,middle_menu_itm_y,common_width,common_height);
	if(chc==PEN) iRectangle(pen_x,middle_menu_itm_y,common_width, common_width);
	if(chc==CURSOR) iRectangle(pointer_x, middle_menu_itm_y, common_width,common_height);
	if(chc==ERASER) iRectangle(eraser_icn_x,middle_menu_itm_y,common_width,common_height);
	if(chc==SHAPE) iRectangle(shapes_icn_x,middle_menu_itm_y,common_width,common_height);
	if(show_gridlines) iRectangle(gridlines_icn_x,middle_menu_itm_y,common_width,common_height);
}

void drawSaveLoad(void){
	is_dark_mode?iShowBMP(save_icn_x,lm_y,"Images\\save_icn_dm.bmp"): iShowBMP(save_icn_x,lm_y,"Images\\save_icn.bmp");
	is_dark_mode? iShowBMP(load_icn_x,lm_y,"Images\\load_icn_dm.bmp"): iShowBMP(load_icn_x,lm_y,"Images\\load_icn.bmp");
	char temp[20];
	for(int j=12, k=0; j<=strlen(file_names[id_of_file]); j++, k++){
			if(k==5||k==11){
				temp[k]=' ';
				j--;
				continue;
			}
			temp[k] = file_names[id_of_file][j];
	}
	is_dark_mode? iSetColor(230,230,230): iSetColor(40,40,40);
	iText(load_icn_x+common_width+gap_x+10, lm_y+15,temp,GLUT_BITMAP_HELVETICA_12);
}

void drawUpperArrows(void){
	is_dark_mode?iSetColor(255-240,255-240,255-240):iSetColor(240,240,240);
	iFilledRectangle(rm_x,rm_y,arr_dim*2+20,arr_dim+10);
	iShowBMP2(left_arrow_x,arrow_y,"Images\\left_arrow.bmp",0xFFFFFF);
	iShowBMP2(right_arrow_x,arrow_y,"Images\\right_arrow.bmp",0xFFFFFF); //need to check
}

void drawColorMenu(void){
	
		for(int i=0;i<5;i++){
			switch(i){
				case 0: iSetColor(255,0,0);
				break;
				case 1: iSetColor(0,255,0);
				break;
				case 2: iSetColor(0,0,255);
				break;
				case 3: iSetColor(255,255,0);
				break; 
				case 4: is_dark_mode? iSetColor(255,255,255):iSetColor(0,0,0);
				break;
			}
			iFilledCircle(color_icn_x-2*3*color_circle_rad+3*i*color_circle_rad,color_icn_y-color_circle_rad*3,color_circle_rad,1000);
		}
	
}

void drawSizeMenu(void){
	int item_x;
	int item_y = middle_menu_itm_y - gap_x - common_height;

	for(int i=0;i<3;i++){
			item_x = (size_icn_x - gap_x - common_width) + i * (gap_x+common_width);
			switch(i){
				case 0: iShowBMP(item_x,item_y,"Images\\size_small_icn.bmp");
				break;
				case 1: iShowBMP(item_x,item_y,"Images\\size_medium_icn.bmp");
				break;
				case 2: iShowBMP(item_x,item_y,"Images\\size_big_icn.bmp");
				break;
			}
		}
}

void drawShapesMenu(void){

	int item_x;
	int item_y = middle_menu_itm_y - gap_x - common_height;
	
	(is_dark_mode && color_r==0 && color_g==0 && color_b==0) ? iSetColor(255-color_r,255-color_g,255-color_b): iSetColor(color_r,color_g,color_b);
	for(int i=0;i<3;i++){
			item_x = (shapes_icn_x - gap_x - common_width) + i * (gap_x+common_width);
			switch(i){
				case 0:
				iLine(item_x + gap_x +10, item_y+common_height,item_x + gap_x -10, item_y );
				break;
				case 1: 
				iRectangle(item_x,item_y,common_width,common_height);
				break;
				case 2:
				iCircle(item_x+common_width/2,item_y+common_height/2,common_height/2,1000);
				break;
			}
	}
	item_y = item_y - (gap_x+common_height);
	for(int i=1;i<3;i++){
			item_x = (shapes_icn_x - gap_x-common_width) + i * (gap_x+common_width);
			switch(i){
				case 1: 
				(is_dark_mode && color_r==0 && color_g==0 && color_b==0)? iSetColor(255-color_r,255-color_g,255-color_b): iSetColor(color_r,color_g,color_b);
				iFilledRectangle(item_x,item_y,common_width,common_height);
				is_dark_mode? iSetColor(255,255,255):iSetColor(0,0,0);
				iRectangle(item_x,item_y,common_width,common_height);
				break;
				case 2: 
				(is_dark_mode && color_r==0 && color_g==0 && color_b==0)? iSetColor(255-color_r,255-color_g,255-color_b): iSetColor(color_r,color_g,color_b);
				iFilledCircle(item_x+common_width/2,item_y+common_height/2,common_height/2,1000);
				is_dark_mode? iSetColor(255,255,255):iSetColor(0,0,0);
				iCircle(item_x+common_width/2,item_y+common_height/2,common_height/2,1000);
				break;
			}
	}
}

void drawCurrentShape(void){

	if(is_draw_line){
		if(is_dark_mode&& lines[current_board][line_index[current_board]].r==0 &&lines[current_board][line_index[current_board]].g==0 && lines[current_board][line_index[current_board]].b==0) iSetColor(255-lines[current_board][line_index[current_board]].r,255-lines[current_board][line_index[current_board]].g,255-lines[current_board][line_index[current_board]].b);
		else	iSetColor(lines[current_board][line_index[current_board]].r,lines[current_board][line_index[current_board]].g,lines[current_board][line_index[current_board]].b);
		iLine(lines[current_board][line_index[current_board]].x1,lines[current_board][line_index[current_board]].y1,lines[current_board][line_index[current_board]].x2,lines[current_board][line_index[current_board]].y2);
	}

	if(is_draw_rectangle){
		if(is_dark_mode&& rectangles[current_board][rectangle_index[current_board]].r==0 &&rectangles[current_board][rectangle_index[current_board]].g==0 && rectangles[current_board][rectangle_index[current_board]].b==0) iSetColor(255-rectangles[current_board][rectangle_index[current_board]].r,255-rectangles[current_board][rectangle_index[current_board]].g,255-rectangles[current_board][rectangle_index[current_board]].b);
		else	iSetColor(rectangles[current_board][rectangle_index[current_board]].r,rectangles[current_board][rectangle_index[current_board]].g,rectangles[current_board][rectangle_index[current_board]].b);
		if(rectangles[current_board][rectangle_index[current_board]].is_filled)
			iFilledRectangle(rectangles[current_board][rectangle_index[current_board]].x,rectangles[current_board][rectangle_index[current_board]].y,rectangles[current_board][rectangle_index[current_board]].dx,rectangles[current_board][rectangle_index[current_board]].dy);

		else
			iRectangle(rectangles[current_board][rectangle_index[current_board]].x,rectangles[current_board][rectangle_index[current_board]].y,rectangles[current_board][rectangle_index[current_board]].dx,rectangles[current_board][rectangle_index[current_board]].dy);
	}
	if(is_draw_circle){
		if(is_dark_mode&& circles[current_board][circle_index[current_board]].r==0 &&circles[current_board][circle_index[current_board]].g==0 && circles[current_board][circle_index[current_board]].b==0) iSetColor(255-circles[current_board][circle_index[current_board]].r,255-circles[current_board][circle_index[current_board]].g,255-circles[current_board][circle_index[current_board]].b);
		else	iSetColor(circles[current_board][circle_index[current_board]].r,circles[current_board][circle_index[current_board]].g,circles[current_board][circle_index[current_board]].b);
		if(circles[current_board][circle_index[current_board]].is_filled)
			iFilledEllipse(circles[current_board][circle_index[current_board]].x,circles[current_board][circle_index[current_board]].y,circles[current_board][circle_index[current_board]].r1,circles[current_board][circle_index[current_board]].r2,1000);
		else
			iEllipse(circles[current_board][circle_index[current_board]].x,circles[current_board][circle_index[current_board]].y,circles[current_board][circle_index[current_board]].r1,circles[current_board][circle_index[current_board]].r2,1000);
	}

}

void show_load_window(){
	load_window_dy = 40 * no_of_files;
	load_window_y = (screenheight-100) - load_window_dy;
	char temp[20];
	is_dark_mode? iSetColor(30,30,30):iSetColor(255,255,255);
	iFilledRectangle(load_window_x,load_window_y, load_window_dx,load_window_dy);
	is_dark_mode? iSetColor(255,255,255): iSetColor(0,0,0);
	iRectangle(load_window_x,load_window_y, load_window_dx,load_window_dy );
	int item_x = load_window_x;
	int item_dx = load_window_dx;
	int item_y;
	for(int i=0; i<no_of_files; i++){
		item_y = load_window_y + load_window_dy -(i+1)*40;
		iRectangle(item_x, item_y, item_dx, 40);
		//for showing the file name with spaces and only the last part
		for(int j=12, k=0; j<=strlen(file_names[i]); j++, k++){
			if(k==5||k==11){
				temp[k]=' ';
				j--;
				continue;
			}
			temp[k] = file_names[i][j];
		}
		iText(item_x+gap_x,item_y+8,temp,GLUT_BITMAP_TIMES_ROMAN_24);
	}
	
}

void iMouseMove(int mx, int my) {
	if(!((mx>menu_x && mx<menu_x+menu_width && my<menu_y+menu_height && my>menu_y)||(mx>left_arrow_x && mx<right_arrow_x+arr_dim && my>arrow_y && my<arrow_y+arr_dim)||(mx>save_icn_x && mx<load_icn_x+common_width&& my>lm_y && my<lm_y+common_height))){
		if(chc==PEN && is_draw_circle==0 && is_draw_rectangle==0 && is_draw_line==0){
			points[current_board][point_indexes[current_board]].size = current_size;
			points[current_board][point_indexes[current_board]].r = color_r;
			points[current_board][point_indexes[current_board]].g = color_g;
			points[current_board][point_indexes[current_board]].b = color_b;
			
			if(n_place==1){
				points[current_board][point_indexes[current_board]].x1 = points[current_board][point_indexes[current_board]].x2 = mx;
				points[current_board][point_indexes[current_board]].y1 = points[current_board][point_indexes[current_board]].y2 = my;
				n_place=0;
			}
			else{
				points[current_board][point_indexes[current_board]].x1 = points[current_board][point_indexes[current_board]-1].x2;
				points[current_board][point_indexes[current_board]].y1 = points[current_board][point_indexes[current_board]-1].y2;
				points[current_board][point_indexes[current_board]].x2 = mx;
				points[current_board][point_indexes[current_board]].y2 = my;
			}
			point_indexes[current_board]++;
		}

		if(is_draw_rectangle){
			if(is_draw_square){
				rectangles[current_board][rectangle_index[current_board]].dx = mx - rectangles[current_board][rectangle_index[current_board]].x;
				rectangles[current_board][rectangle_index[current_board]].dy = (my - rectangles[current_board][rectangle_index[current_board]].y)<0? -abs(rectangles[current_board][rectangle_index[current_board]].dx):abs(rectangles[current_board][rectangle_index[current_board]].dx) ;
			}
			else{
			rectangles[current_board][rectangle_index[current_board]].dx = mx - rectangles[current_board][rectangle_index[current_board]].x;
			rectangles[current_board][rectangle_index[current_board]].dy = my - rectangles[current_board][rectangle_index[current_board]].y;
			}
			
		}

		if(is_draw_circle){
			if(is_draw_uniform_circle){
				circles[current_board][circle_index[current_board]].r1 = (mx - circles[current_board][circle_index[current_board]].x);
				circles[current_board][circle_index[current_board]].r2 = (my- circles[current_board][circle_index[current_board]].y)<0?-abs(circles[current_board][circle_index[current_board]].r1):abs(circles[current_board][circle_index[current_board]].r1);
			}

			else{
				circles[current_board][circle_index[current_board]].r1 = (mx - circles[current_board][circle_index[current_board]].x);
				circles[current_board][circle_index[current_board]].r2 = (my- circles[current_board][circle_index[current_board]].y);
			}
			
		}

		if(is_draw_line){
			lines[current_board][line_index[current_board]].x2 = mx;
			lines[current_board][line_index[current_board]].y2 = my;

		}

		if(chc==ERASER){
			erased_points[current_board][eraser_index[current_board]].x = mx;
			erased_points[current_board][eraser_index[current_board]].y = my;
			eraser_index[current_board]++;
		}
	}
}

void iMouse(int button, int state, int mx, int my) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if(is_load_clicked){
			if(mx>load_window_x && mx<load_window_x+load_window_dx && my>load_window_y && my<load_window_y+load_window_dy)
				load_file(get_load_index(my));
			is_load_clicked=0;
		}
		else{
			if(chc==TEXT && !((mx>menu_x && mx<menu_x+menu_width && my<menu_y+menu_height && my>menu_y)||(mx>left_arrow_x && mx<right_arrow_x+arr_dim && my>arrow_y && my<arrow_y+arr_dim)||(mx>save_icn_x && mx<load_icn_x+common_width&& my>lm_y && my<lm_y+common_height))){
				if(!(mx>menu_x && mx<menu_x+menu_height && my<menu_y+menu_height && my>menu_y)){
				texts[current_board][text_index[current_board]].r = color_r;
				texts[current_board][text_index[current_board]].g = color_g;
				texts[current_board][text_index[current_board]].b = color_b;
				texts[current_board][text_index[current_board]].x = mx;
				texts[current_board][text_index[current_board]].y = my;
				texts[current_board][text_index[current_board]].size = current_size;
				chc=PEN;
				text_active=1;
				}
			}

			//For checking if color button is clicked
			if(mx>(color_icn_x-color_circle_rad)&& mx< (color_icn_x+color_circle_rad)&&my<color_icn_y+color_circle_rad && my> color_icn_y-color_circle_rad){
				is_color_menu=!is_color_menu;
			}
			else if(is_color_menu==1){
				int item_cent;
				if(my<(color_circle_rad+color_icn_y-3*color_circle_rad)&&my>(-color_circle_rad*3+color_icn_y-color_circle_rad)){
					for(int i=0; i<5;i++){
						item_cent= color_icn_x-6*color_circle_rad+3*i*color_circle_rad;
						if(mx>(item_cent-color_circle_rad)&&mx<(item_cent+color_circle_rad)){
							switch (i){
								case 0:
									color_r=255;
									color_g=0;
									color_b=0;
									break;
								case 1:
									color_g=255;
									color_r=0;
									color_b=0;
									break;
								case 2:
									color_b=255;
									color_r=0;
									color_g=0;
									break;
								case 3:
									color_r=255;
									color_g = 255;
									color_b=0;
									break;
								case 4:
									color_r=0;
									color_b=0;
									color_g=0;
									break;
								}
							break; //breaking the loop
						}
					}
					
				}
				is_color_menu=0;
				//when after erasing, user select color automatically set choice to pen
				if(chc==ERASER){
					current_layer[current_board]++;
					chc=PEN;
				}
			}
			

			//for checking if pen button is clicked
			if(mx>pen_x && mx<(pen_x+common_width) && my>middle_menu_itm_y && my<(middle_menu_itm_y+common_width)){
				is_draw_circle = is_draw_line = is_draw_rectangle=0;
				if(chc==ERASER) current_layer[current_board]++;
				chc=PEN;
			}
			if(is_draw_rectangle){
				rectangles[current_board][rectangle_index[current_board]].x=mx;
				rectangles[current_board][rectangle_index[current_board]].y=my;			
			}
			else if(is_draw_circle){
				circles[current_board][circle_index[current_board]].x=mx;
				circles[current_board][circle_index[current_board]].y = my;
			}
			else if(is_draw_line){
				lines[current_board][line_index[current_board]].x1=mx;
				lines[current_board][line_index[current_board]].y1 = my;
				lines[current_board][line_index[current_board]].x2=mx;
				lines[current_board][line_index[current_board]].y2 = my;
			}
				
			if(mx>pointer_x && mx<(pointer_x+common_width) && my>middle_menu_itm_y && my<(middle_menu_itm_y+common_width)){
				if(chc==ERASER) current_layer[current_board]++;
				chc = CURSOR;
			}
			
			//check if right arr or left arr is clicked
			if(mx>left_arrow_x && mx<left_arrow_x+arr_dim && my>arrow_y && my<arrow_y+arr_dim && current_board!=0) current_board--;
			if(mx>right_arrow_x && mx<right_arrow_x+arr_dim && my>arrow_y && my<arrow_y+arr_dim && current_board<9) current_board++;
			
			//check if text icon is clicked
			if(mx>text_icn_x && mx<text_icn_x+common_width && my>middle_menu_itm_y && my<middle_menu_itm_y+common_height){
				if(chc==ERASER) current_layer[current_board]++;
				chc=TEXT;
			}

			//check if size menu is selected
			if(mx>size_icn_x && mx<(size_icn_x+common_width) && my>middle_menu_itm_y && my<(middle_menu_itm_y+common_height)){
				is_size_menu = !is_size_menu;
				is_shapes_menu=0;
			}
			else if(is_size_menu==1){
				int item_x;
				if(my<(middle_menu_itm_y-gap_x)&&my>(middle_menu_itm_y-gap_x-common_height)){
					for(int i=0; i<3;i++){
						item_x= (size_icn_x - gap_x - common_width) + i * (gap_x+common_width);
						if(mx>(item_x)&&mx<(item_x+common_width)){
							current_size=i+1;
							break;
						}
					}
				}
				is_size_menu=0;
			}
			if(mx>eraser_icn_x && mx<eraser_icn_x+common_width && my>middle_menu_itm_y && my<middle_menu_itm_y+common_height){
				chc=ERASER;
			}
			//check if gridlines button is pressed
			if(mx>gridlines_icn_x && mx<(gridlines_icn_x+common_width) && my>middle_menu_itm_y && my<(middle_menu_itm_y+common_height)){
				show_gridlines = !show_gridlines;
			}

			//check if CFB is clicked
			if(mx>CFB_x && mx<CFB_x+common_width && my>middle_menu_itm_y && my<middle_menu_itm_y+common_height){
				clear_full_board(current_board);
			}

			if(mx>dark_mode_x && mx<dark_mode_x+common_width && my>middle_menu_itm_y && my<middle_menu_itm_y+common_height){
				is_dark_mode = !is_dark_mode;
			}
		}
		if(mx>save_icn_x && mx<(save_icn_x+common_width) && my>lm_y && my<(lm_y+common_height)){
			save_current_file();
		}
		if(mx>load_icn_x && mx<(load_icn_x+common_width) && my>lm_y && my<(lm_y+common_height)){
			is_load_clicked=1;
			chc=INACTIVE;
		}


	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		if(chc==PEN)	n_place=1;

		if(mx>shapes_icn_x && mx<(shapes_icn_x+common_width) && my>middle_menu_itm_y && my<(middle_menu_itm_y+common_height)){
			if(is_shapes_menu) chc=PEN;
			is_shapes_menu = !is_shapes_menu;
		}
		if(is_shapes_menu==0 && !(mx>menu_x && mx<(menu_x+menu_width) && my<(menu_y+menu_height) && my>menu_y)){
			if(is_draw_rectangle){
				is_draw_rectangle=0;
				is_draw_square=0;
				rectangle_index[current_board]++;
			}
			else if(is_draw_circle){
				is_draw_circle=0;
				is_draw_uniform_circle=0;
				circle_index[current_board]++;
			}
			else if(is_draw_line){
				is_draw_line=0;
				line_index[current_board]++;
			}
			if(chc==SHAPE){
				current_layer[current_board]++;
				chc=PEN;
			}
			
		}
		else if(is_shapes_menu==1){
			int t;
			if(my<(middle_menu_itm_y-gap_x)&&my>(middle_menu_itm_y-gap_x-common_height)){
				for(int i=0; i<3;i++){
					t= (shapes_icn_x - gap_x - common_width) + i * (gap_x+common_width);
					if(mx>(t)&&mx<(t+common_width)){
						switch (i){
						case 0:
							is_draw_line = 1;
							lines[current_board][line_index[current_board]].r = color_r;
							lines[current_board][line_index[current_board]].g = color_g;
							lines[current_board][line_index[current_board]].b = color_b;
							if(chc==ERASER) current_layer[current_board]++;
							chc = SHAPE;
							break;
						case 1:
							is_draw_rectangle=1;
							rectangles[current_board][rectangle_index[current_board]].r = color_r;
							rectangles[current_board][rectangle_index[current_board]].g = color_g;
							rectangles[current_board][rectangle_index[current_board]].b = color_b;
							if(chc==ERASER) current_layer[current_board]++;
							chc=SHAPE;
							break;
						case 2: 
							is_draw_circle=1;
							circles[current_board][circle_index[current_board]].r = color_r;
							circles[current_board][circle_index[current_board]].g = color_g;
							circles[current_board][circle_index[current_board]].b = color_b;
							if(chc==ERASER) current_layer[current_board]++;
							chc=SHAPE;
							break;
						}
						is_shapes_menu=0;
						break; //breaking the loop
					}
				}
			}

			if(my<(middle_menu_itm_y-gap_x*2-common_height)&&my>(middle_menu_itm_y-gap_x*2-common_height*2)){
				for(int i=1; i<3;i++){
					t= (shapes_icn_x - gap_x - common_width) + i * (gap_x+common_width);
					if(mx>(t)&&mx<(t+common_width)){
						switch (i)
						{
						case 1:
							is_draw_rectangle=1;
							rectangles[current_board][rectangle_index[current_board]].r = color_r;
							rectangles[current_board][rectangle_index[current_board]].g = color_g;
							rectangles[current_board][rectangle_index[current_board]].b = color_b;
							rectangles[current_board][rectangle_index[current_board]].is_filled=1;
							if(chc==ERASER) current_layer[current_board]++;
							chc = SHAPE;
							break;
						case 2: 
							is_draw_circle=1;
							circles[current_board][circle_index[current_board]].r = color_r;
							circles[current_board][circle_index[current_board]].g = color_g;
							circles[current_board][circle_index[current_board]].b = color_b;
							circles[current_board][circle_index[current_board]].is_filled=1;
							if(chc==ERASER) current_layer[current_board]++;
							chc=SHAPE;
							break;
						}
						is_shapes_menu=0;
						break;
					}
				}
			}
			
		}
	}
}

void iKeyboard(unsigned char key) {
	
	if(text_active){
		if(key!=13&& key !=8){
			texts[current_board][text_index[current_board]].text[text_char_id] = key;
			text_char_id++;
		}
		else if (key==13){
			text_active=0;
			text_index[current_board]++;
			text_char_id=0;
		}
		else if(key==8){
			if(text_char_id>0)
				text_char_id--;
			texts[current_board][text_index[current_board]].text[text_char_id] = ' ';
		}
	}
	else if(key=='d') is_dark_mode=!is_dark_mode;
	else if(key =='g') show_gridlines=!show_gridlines;
	else if(key=='p'){
		if(chc==ERASER) current_layer[current_board]++;
		chc=PEN;
	} 
	else if(key == 'e') chc=ERASER;
	else if(key =='t') chc=TEXT;
	else if(key =='s'){
		if(is_draw_rectangle)
			is_draw_square=1;
		else if(is_draw_circle)
			is_draw_uniform_circle=1;
		else{
			is_shapes_menu=!is_shapes_menu;
		}
	}
	else if(key=='c') chc=CURSOR;
	else if(key=='i') current_size = (current_size+1)%3;
	else if (key==127) clear_full_board(current_board);
	
}

void iSpecialKeyboard(unsigned char key) {
	if(key==GLUT_KEY_UP) if(current_board!=0) current_board--;
	if(key==GLUT_KEY_DOWN) if(current_board!=9) current_board++;
	if(key==GLUT_KEY_PAGE_DOWN) current_board=9;
	if(key==GLUT_KEY_PAGE_UP) current_board=0;
}

void clear_full_board(int id){
	
	for(int j=0; j<point_indexes[id];j++)	erase_point(&points[id][j]);
	for(int j=0; j<text_index[id];j++)	erase_text(&texts[id][j]);
	for(int j=0; j<line_index[id];j++)	erase_line(&lines[id][j]);
	for(int j=0; j<rectangle_index[id];j++)	erase_rectangle(&rectangles[id][j]);
	for(int j=0; j<circle_index[id];j++)	erase_circle(&circles[id][j]);
	for(int j=0; j<eraser_index[id];j++) erase_erasers(&erased_points[id][j]);
	for(int j=0; j<=current_layer[id];j++) erase_layers(&layer_seps[id][j]);
	text_index[id] = point_indexes[id] = line_index[id] = rectangle_index[id]=circle_index[id] = eraser_index[id]=0;
	current_layer[id]=0;
	n_place=1;
}

void erase_point(struct POINT_ *p){
	p->b=0;
	p->g=0;
	p->r=0;
	p->x1=0;
	p->x2=0;
	p->y1=0;
	p->y2=0;
}

void erase_text(struct TEXT_ *t){
	t->b=0;
	t->g=0;
	t->r=0;
	t->x=0;
	t->y=0;
	t->size=0;
	int len=0;
	char *c = (t->text);
	while(*c!='\0'){
		c++;
		len++;
	}
	for(int i=0; i<len;i++){
		t->text[i]='\0';
	}
}

void erase_line(struct LINE_ *l){
	l->b=0;
	l->g=0;
	l->r =0;
	l->x1=0;
	l->x2=0;
	l->y1=0;
	l->y2=0;
}

void erase_rectangle(struct RECTANGLE_ *r){
	r->b=0;
	r->g=0;
	r->r =0;
	r->x=0;
	r->y=0;
	r->dx=0;
	r->dy=0;
	r->is_filled=0;
}

void erase_circle(struct CIRCLE_ *c){
	c->b=0;
	c->g=0;
	c->r =0;
	c->x=0;
	c->y=0;
	c->r1=0;
	c->r2=0;
	c->is_filled=0;
}

void erase_erasers(struct ERASE_P_ *e){
	e->x=0;
	e->y=0;
}

void erase_layers(struct LAYER_SEPARATION_ *ls){
	ls->c_sep=0;
	ls->e_sep=0;
	ls->l_sep=0;
	ls->p_sep=0;
	ls->r_sep=0;
	ls->t_sep=0;
}

void save_current_file(){
	
	sprintf(file_names[id_of_file],"Saved_files\\WhiteBoard%d",id_of_file+1);
	if(id_of_file>=no_of_files) no_of_files++;
	if((save_load_file=fopen(file_names[id_of_file],"w"))==NULL){
		printf("Error opening file to save!");
		exit(-5);
	}

	//saving the points
	for(int i=0; i<10; i++){
		fprintf(save_load_file, "\n\nFor Board: %d\n", i+1);
		fprintf(save_load_file,"Layers:\nLayer Index: %d\n",current_layer[i]);
		for(int j=0; j<=current_layer[i];j++){
			fprintf(save_load_file,"For Layer %d:\nPS:%d TS:%d LS:%d RS:%d CS:%d ES:%d\n",j,layer_seps[i][j].p_sep,layer_seps[i][j].t_sep,layer_seps[i][j].l_sep,layer_seps[i][j].r_sep,layer_seps[i][j].c_sep,layer_seps[i][j].e_sep);
		}
		
		fprintf(save_load_file,"Points:\nPoint Index: %d\n",point_indexes[i]);

		for(int j=0; j<point_indexes[i];j++){
			fprintf(save_load_file,"x1=%d x2=%d y1=%d y2=%d r=%d g=%d b=%d size=%d\n",points[i][j].x1,points[i][j].x2,points[i][j].y1,points[i][j].y2,points[i][j].r,points[i][j].g,points[i][j].b,points[i][j].size);
		}
		fprintf(save_load_file,"Texts:\nText Index: %d\n",text_index[i]);
		for(int j=0; j<text_index[i];j++){
			fprintf(save_load_file, "x=%d y=%d r=%d g=%d b=%d size=%d s=%s\n",texts[i][j].x,texts[i][j].y,texts[i][j].r,texts[i][j].g,texts[i][j].b,texts[i][j].size,texts[i][j].text);
		}
		fprintf(save_load_file,"Lines:\nLine Index: %d\n",line_index[i]);
		for(int j=0; j<line_index[i];j++){
			fprintf(save_load_file, "x1=%d x2=%d y1=%d y2=%d r=%d g=%d b=%d\n",lines[i][j].x1,lines[i][j].x2,lines[i][j].y1,lines[i][j].y2,lines[i][j].r,lines[i][j].g,lines[i][j].b);
		}

		fprintf(save_load_file,"Rectangles:\nRectangle Index: %d\n",rectangle_index[i]);
		for(int j=0; j<rectangle_index[i];j++){
			fprintf(save_load_file, "x=%d dx=%d y=%d dy=%d r=%d g=%d b=%d fill=%d\n",rectangles[i][j].x,rectangles[i][j].dx,rectangles[i][j].y,rectangles[i][j].dy,rectangles[i][j].r,rectangles[i][j].g,rectangles[i][j].b,rectangles[i][j].is_filled);
		}

		fprintf(save_load_file,"Circles:\nCircle Index: %d\n",circle_index[i]);
		for(int j=0; j<circle_index[i];j++){
			fprintf(save_load_file, "x=%d y=%d r1=%d r2=%d r=%d g=%d b=%d fill=%d\n",circles[i][j].x,circles[i][j].y,circles[i][j].r1,circles[i][j].r2,circles[i][j].r,circles[i][j].g,circles[i][j].b,circles[i][j].is_filled);
		}

		fprintf(save_load_file,"Erased:\nEraser Index: %d\n",eraser_index[i]);

		for(int j=0; j<eraser_index[i];j++){
			fprintf(save_load_file,"x=%d y=%d\n",erased_points[i][j].x, erased_points[i][j].y);
		}
	}

	fclose(save_load_file);
	updateFileInfo();
}

int get_load_index(int my){
	for(int i=0; i<no_of_files;i++){
		if(my>(load_window_y+load_window_dy-(i+1)*40) && my<(load_window_y+load_window_dy-(i)*40)){
			//printf("\n%d",i);
			return i;
		}	
	}
	return -1;
}

void load_file(int index){
	current_board=0;
	color_r=0;
	color_g=0;
	color_b=0;
	current_size=1;
	if(index==-1) return;
	for(int i=0;i<10;i++) clear_full_board(i);
	if ((save_load_file = fopen(file_names[index],"r"))==NULL){
		printf("Error opening file to load");
		exit(-5);
	}
	id_of_file = index;
	int board_id,temp;
	char t;
	for(int i=0; i<10; i++){
		fscanf(save_load_file, "\n\nFor Board: %d\n", &board_id);
		fscanf(save_load_file,"Layers:\nLayer Index: %d\n",&current_layer[i]);
		for(int j=0; j<=current_layer[i];j++){
			fscanf(save_load_file,"For Layer %d:\nPS:%d TS:%d LS:%d RS:%d CS:%d ES:%d\n",&temp,&layer_seps[i][j].p_sep,&layer_seps[i][j].t_sep,&layer_seps[i][j].l_sep,&layer_seps[i][j].r_sep,&layer_seps[i][j].c_sep,&layer_seps[i][j].e_sep);
		}
		board_id--;
		fscanf(save_load_file,"Points:\nPoint Index: %d\n",&point_indexes[board_id]);
		for(int j=0; j<point_indexes[board_id];j++){
			fscanf(save_load_file,"x1=%d x2=%d y1=%d y2=%d r=%d g=%d b=%d size=%d\n",&points[board_id][j].x1,&points[board_id][j].x2,&points[board_id][j].y1,&points[board_id][j].y2,&points[board_id][j].r,&points[board_id][j].g,&points[board_id][j].b,&points[board_id][j].size);
		}
		fscanf(save_load_file,"Texts:\nText Index: %d\n",&text_index[board_id]);
		for(int j=0; j<text_index[board_id];j++){
			fscanf(save_load_file, "x=%d y=%d r=%d g=%d b=%d size=%d s=%[^\n]\n",&texts[board_id][j].x,&texts[board_id][j].y,&texts[board_id][j].r,&texts[board_id][j].g,&texts[board_id][j].b,&texts[board_id][j].size,&texts[board_id][j].text);
		}
		fscanf(save_load_file,"Lines:\nLine Index: %d\n",&line_index[board_id]);
		for(int j=0; j<line_index[board_id];j++){
			fscanf(save_load_file, "x1=%d x2=%d y1=%d y2=%d r=%d g=%d b=%d\n",&lines[board_id][j].x1,&lines[board_id][j].x2,&lines[board_id][j].y1,&lines[board_id][j].y2,&lines[board_id][j].r,&lines[board_id][j].g,&lines[board_id][j].b);
		}

		fscanf(save_load_file,"Rectangles:\nRectangle Index: %d\n",&rectangle_index[board_id]);
		for(int j=0; j<rectangle_index[board_id];j++){
			fscanf(save_load_file, "x=%d dx=%d y=%d dy=%d r=%d g=%d b=%d fill=%d\n",&rectangles[board_id][j].x,&rectangles[board_id][j].dx,&rectangles[board_id][j].y,&rectangles[board_id][j].dy,&rectangles[board_id][j].r,&rectangles[board_id][j].g,&rectangles[board_id][j].b,&rectangles[board_id][j].is_filled);
		}

		fscanf(save_load_file,"Circles:\nCircle Index: %d\n",&circle_index[board_id]);
		for(int j=0; j<circle_index[board_id];j++){
			fscanf(save_load_file, "x=%d y=%d r1=%d r2=%d r=%d g=%d b=%d fill=%d\n",&circles[board_id][j].x,&circles[board_id][j].y,&circles[board_id][j].r1,&circles[board_id][j].r2,&circles[board_id][j].r,&circles[board_id][j].g,&circles[board_id][j].b,&circles[board_id][j].is_filled);
		}

		fscanf(save_load_file,"Erased:\nEraser Index: %d\n",&eraser_index[i]);

		for(int j=0; j<eraser_index[i];j++){
			fscanf(save_load_file,"x=%d y=%d\n",&erased_points[i][j].x, &erased_points[i][j].y);
		}
	}

	fclose(save_load_file);

}


int main() {
	initBoard();
	iInitialize(screenwidth, screenheight, "White Board");
	return 0;
}
