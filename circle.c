#include "mlx.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define MLX_ERROR 1



typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}			t_img;

typedef struct s_data
{
	void	*mlx;
	void	*window;
	t_img	img;

	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		dx;
	int		dy;
	int		sx;
	int		sy;
	int		err;

	int speed;

}			t_data;

void	my_mlx_pixel_put(t_img *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	draw_line(t_img *data, int x0, int y0, int x1, int y1)
{
	int	dx;
	int	dy;
	int	sx;
	int	sy;
	int	err;
	int	e2;

	dx = abs(x1 - x0);
	dy = abs(y1 - y0);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = dx - dy;
	while ((x0 != x1) || (y0 != y1))
	{
		my_mlx_pixel_put(data, x0, y0, 0x00000000);
		e2 = 2 * err;
		if (e2 > -dy)
		{
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 < dx)
		{
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}

void	screenplay(t_img *data, int x, int y)
{
	for (int i = 0; i < WINDOW_WIDTH; i++)
		for (int j = 0; j < WINDOW_HEIGHT; j++)
		{
			if (pow((i - x), 2) + pow((j - y), 2) < pow(50, 2))
				my_mlx_pixel_put(data, i, j, 0xFFFEA21);
			else
				my_mlx_pixel_put(data, i, j, 0x00FFFFFF);
		}
	draw_line(data, 0, 200, 100, 200);
	draw_line(data, 100, 200, 850, 650);
	draw_line(data, 850, 650, 1000, 650);
}

// 120 - the center in the x axis
// 153 - the center in the y axis

// 865 - the center in the x axis
// 600 - the center in the y axis

int	rendering(t_data *data)
{
	int	e2;

	if (data->x0 % data->speed == 0)
        screenplay(&data->img, data->x0, data->y0);
    
	if ((data->x0 != data->x1) || (data->y0 != data->y1))
	{
		//while (data->x0 % 5 != 0)
		//{
			e2 = 2 * data->err;
			if (e2 > -1 * data->dy)
			{
				data->err = data->err - data->dy;
				data->x0 = data->x0 + data->sx;
			}
			if (e2 < data->dx)
			{
				data->err = data->err + data->dx;
				data->y0 = data->y0 + data->sy;
			}
		//}
	}
	else
	{
		data->x0 = 120;
		data->x1 = 865;
		data->y0 = 153;
		data->y1 = 600;
		data->dx = abs(865 - 120);
		data->dy = abs(600 - 153);
		data->sx = (120 < 865) ? 1 : -1;
		data->sy = (153 < 600) ? 1 : -1;
		data->err = data->dx - data->dy;
	}
	mlx_put_image_to_window(data->mlx, data->window, data->img.img, 0, 0);
	// animate_circle(&data->img, 120, 153, 865, 600);
	// void animate_circle(t_img *data, int x0, int y0, int x1, int y1)
}

#include <stdio.h>

int mouse_press(int button, int x, int y, t_data *data)
{
    (void)y;
	(void)x;
	(void)button;

	// printf("I was here\n");
	data->x0 = 120;
	data->x1 = 865;
	data->y0 = 153;
	data->y1 = 600;
	data->dx = abs(865 - 120);
	data->dy = abs(600 - 153);
	data->sx = (120 < 865) ? 1 : -1;
	data->sy = (153 < 600) ? 1 : -1;
	data->err = data->dx - data->dy;
	data->speed *= 5;
}

int	main(void)
{
	t_data data;
	data.speed = 3;
	data.x0 = 120;
	data.x1 = 865;
	data.y0 = 153;
	data.y1 = 600;
	data.dx = abs(865 - 120);
	data.dy = abs(600 - 153);
	data.sx = (120 < 865) ? 1 : -1;
	data.sy = (153 < 600) ? 1 : -1;
	data.err = data.dx - data.dy;

	data.mlx = mlx_init();
	if (data.mlx == NULL)
		return (MLX_ERROR);
	data.window = mlx_new_window(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT,
			"Circle Life");
	if (data.window == NULL)
		return (MLX_ERROR);
	data.img.img = mlx_new_image(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (data.img.img == NULL)
		return (MLX_ERROR);
	data.img.addr = mlx_get_data_addr(data.img.img, &data.img.bits_per_pixel,
			&data.img.line_length, &data.img.endian);
	mlx_loop_hook(data.mlx, &rendering, &data);
    mlx_hook(data.window, 4, 1L<<2, &mouse_press, &data);
	mlx_loop(data.mlx);
}