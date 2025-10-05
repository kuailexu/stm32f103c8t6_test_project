#include <math.h>
#include "common.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "font.h"


u16 LCD_BGR2RGB(uint16_t c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}


typedef struct {
    int integer_digits[5];  
    int decimal_digits[3];  
    int integer_length;     
    int decimal_length;     
    bool has_decimal;      
    bool is_valid;          
} NumberParts;


NumberParts parse_number(double num) {
    NumberParts result = {{0}, {0}, 0, 0, false, true};
    int integer = (int)num;
    if (integer < 0) integer = -integer; 
    if (integer > 99999) {
        result.is_valid = false;
        return result;
    }
    int temp = integer;
    result.integer_length = 0;
    do {
        result.integer_digits[4 - result.integer_length] = temp % 10; 
        temp /= 10;
        result.integer_length++;
    } while (temp > 0 && result.integer_length < 5);
    double decimal = fabs(num - (int)num); 
    if (decimal > 0) {
        result.has_decimal = true;
        decimal = round(decimal * 1000) / 1000; 
        int decimal_int = (int)(decimal * 1000);
        result.decimal_length = 0;
        for (int i = 0; i < 3; i++) {
            result.decimal_digits[i] = (decimal_int / (int)pow(10, 2 - i)) % 10;
            if (result.decimal_digits[i] != 0 || i < 2) {
                result.decimal_length++;
            }
        }
    }
    return result;
}


void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)   
{
	int dx,dy,dx2,dy2,x_inc,y_inc,error,index;          
	Lcd_SetXY(x0,y0);
	dx = x1-x0;
	dy = y1-y0;
	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 

	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)
	{
		error = dy2 - dx; 
		for (index=0; index <= dx; index++)
		{
			Gui_DrawPoint(x0,y0,Color);
			if (error >= 0) 
			{
				error-=dx2;
				y0+=y_inc;
			} 
			error+=dy2;
			x0+=x_inc;
		}
	} 
	else
	{
		error = dx2 - dy; 
		for (index=0; index <= dy; index++)
		{
			Gui_DrawPoint(x0,y0,Color);
			if (error >= 0)
			{
				error-=dy2;
				x0+=x_inc;
			} 
			error+=dx2;
			y0+=y_inc;
		} 
	} 
}


void Gui_DrawCurve(u16 x, u16 y,u16 size_x, u16 size_y, int16_t *data, uint8_t scale, u16 color)
{	
	Lcd_Clear_Region(x, y, size_x*scale, size_y*scale, GRAY0);
	for (uint8_t i = 0; i < size_x - 1; i++) {
		uint16_t x0 = x + i * scale;
		uint16_t y0 = y + (size_y / 2) - data[i] * scale; 
		uint16_t x1 = x + (i + 1) * scale;
		uint16_t y1 = y + (size_y / 2) - data[i + 1] * scale; 
		Gui_DrawLine(x0, y0, x1, y1, color);
	}
}
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}


void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else 
		{
		

			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 ) 
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		    for(i=0;i<16;i++)
			for(j=0;j<8;j++) 
				{
			    	if(asc16[k*16+i]&(0x80>>j))	
					Gui_DrawPoint(x+j,y+i,fc);
					else 
					{
						if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else 
		{

			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								Gui_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								Gui_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
		}
	}
}
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
	unsigned char i,j,k,c;
	//lcd_text_any(x+94+i*42,y+34,32,32,0x7E8,0x0,sz32,knum[i]);
//	w=w/8;
    for(i=0;i<32;i++)
	{
		for(j=0;j<4;j++) 
		{
			c=*(sz32+num*32*4+i*4+j);
			for (k=0;k<8;k++)	
			{
		    	if(c&(0x80>>k))	Gui_DrawPoint(x+j*8+k,y+i,fc);
				else {
					if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
				}
			}
		}
	}
}
void Gui_DrawFont_Num16(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
	unsigned char i,k,c;
	for(i=0;i<16;i++)
	{
		c=*(asc16+16*16+num*16+i);
		for (k=0;k<8;k++)	
		{
				if(c&(0x80>>k))	
					Gui_DrawPoint(x+k,y+i,fc);
				else {
					if (fc!=bc) 
						Gui_DrawPoint(x+k,y+i,bc);
				}
		}
	}
}

void LCD_Num_16x8(u16 x,u16 y,u16 fc,u16 bc,float num)
{
		NumberParts np = parse_number(num);
		if (!np.has_decimal)
		{
			for(int i = 0; i<np.integer_length ; i++)
			{
				Gui_DrawFont_Num16(x+i*8,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
			}
		}	
		else
		{
			for(int i = 0; i<np.integer_length ; i++)
			{
				Gui_DrawFont_Num16(x+i*8,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
			}
			Gui_DrawFont_Str16(x+np.integer_length*8,y,fc,bc,".");
			for(int i = 0; i<np.decimal_length ; i++)
			{
				Gui_DrawFont_Num16(x+(np.integer_length+1)*8+i*8,y,fc,bc,np.decimal_digits[i]);
			}
		}
}
void Gui_DrawFont_Num6(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
	unsigned char i,k,c;
	for(i=0;i<6;i++)
	{
		c=*(asc6+16*6+num*6+i);
		for (k=0;k<8;k++)	
		{
				if(c&(0x01<<k))	
					Gui_DrawPoint(x+i,y+k,fc);
				else {
					if (fc!=bc) 
						Gui_DrawPoint(x+i,y+k,bc);
				}
		}
	}
}
// void LCD_Num_8x6(ContentItem_t *item, uint8_t isFocused)
// {
// 	u16 x = item->x;
// 	u16 y = item->y;
// 	u16 fc = RED;
// 	u16 bc = GRAY0;
// 	float *last_num = (float *)item->last_data; // 指向上次显示的数值
// 	float num = *(float *)item->data; // 当前数值
// 	NumberParts np = parse_number(num);
// 	float last_num1 = *last_num;
// 	NumberParts last_np = parse_number(last_num1);
// 	if ((last_np.integer_length != np.integer_length)&&(last_num!=NULL))
// 			Lcd_Clear_Region(x, y, 9*6, 8, GRAY0);
// 	if (!np.has_decimal)
// 	{	
// 		u16 x_start = (x+27-(np.integer_length*6/2));
// 		for(int i = 0; i<np.integer_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+i*6,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
// 		}
// 	}	
// 	else
// 	{	
// 		u16 x_start = (x+27-((np.integer_length+np.decimal_length+1)*6/2));
// 		for(int i = 0; i<np.integer_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+i*6,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
// 		}
// 		Gui_DrawFont_Str8x6(x_start+np.integer_length*6,y,fc,bc,".");
// 		for(int i = 0; i<np.decimal_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+(np.integer_length+1)*6+i*6,y,fc,bc,np.decimal_digits[i]);
// 		}
// 	}
// }
// void LCD_Num_8x6(u16 x,u16 y,u16 fc,u16 bc,float num,float* last_num)
// {
// 	NumberParts np = parse_number(num);
// 	float last_num1 = *last_num;
// 	NumberParts last_np = parse_number(last_num1);
// 	if ((last_np.integer_length != np.integer_length)&&(last_num!=NULL))
// 			Lcd_Clear_Region(x, y, 9*6, 8, GRAY0);
// 	if (!np.has_decimal)
// 	{	
// 		u16 x_start = (x+27-(np.integer_length*6/2));
// 		for(int i = 0; i<np.integer_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+i*6,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
// 		}
// 	}	
// 	else
// 	{	
// 		u16 x_start = (x+27-((np.integer_length+np.decimal_length+1)*6/2));
// 		for(int i = 0; i<np.integer_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+i*6,y,fc,bc,np.integer_digits[4-np.integer_length+i+1]);
// 		}
// 		Gui_DrawFont_Str8x6(x_start+np.integer_length*6,y,fc,bc,".");
// 		for(int i = 0; i<np.decimal_length ; i++)
// 		{
// 			Gui_DrawFont_Num6(x_start+(np.integer_length+1)*6+i*6,y,fc,bc,np.decimal_digits[i]);
// 		}
// 	}
// }
void Gui_DrawFont_Str16(u16 x, u16 y, u16 fc, u16 bc, const char *str)
{
    unsigned char i, k, c;
    u16 x0 = x; 
    while(*str != '\0') 
    {
        
        for(i = 0; i < 16; i++) 
        {
            
            c = *(asc16 + (*str - 0x20) * 16 + i); 
            for(k = 0; k < 8; k++)
            {
                if(c & (0x80 >> k)) 
                    Gui_DrawPoint(x + k, y + i, fc); 
                else if(fc != bc) 
                    Gui_DrawPoint(x + k, y + i, bc); 
            }
        }
        x += 8; 
        str++;  
        if(x > LCD_WIDTH - 8) { 
            x = x0; 
            y += 16;
        }
    }
}
void Gui_DrawFont_Str8x6(u16 x, u16 y, u16 fc, u16 bc, const char *str)
{
    unsigned char i, k, c;
    u16 x0 = x; 
    while(*str != '\0') 
    {
        
        for(i = 0; i < 6; i++) 
        {
            
            c = *(asc6 + (*str - 0x20) * 6 + i); 
            
            for(k = 0; k < 8; k++)
            {
                if(c & (0x01 << k)) 
                    Gui_DrawPoint(x + i, y + k, fc); 
                else if(fc != bc) 
                    Gui_DrawPoint(x + i, y + k, bc); 
            }
        }
        x += 6; 
        str++;  
        if(x > LCD_WIDTH - 6) { 
            x = x0; 
            y += 8; 
        }
    }
}
