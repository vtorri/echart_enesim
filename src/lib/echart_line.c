/* Echart - Chart rendering library
 * Copyright (C) 2013 Vincent Torri
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Enesim.h>

#include "Echart.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/**
 * @cond LOCAL
 */

struct _Echart_Line
{
    Echart_Chart *chart;
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

EAPI Enesim_Renderer *
echart_line_new(Echart_Chart *chart)
{
    const Echart_Data *data;
    const Echart_Data_Item *absciss;
    const Echart_Data_Item *item;
    Enesim_Renderer *c;
    Enesim_Renderer *bg;
    Enesim_Renderer *r;
    Enesim_Renderer_Compound_Layer *l;
    Enesim_Path *p;
    double avmin;
    double avmax;
    int w;
    int h;
    int grid_x_nbr;
    int grid_y_nbr;
    unsigned int i;
    unsigned int j;

    if (!chart)
        return NULL;

    echart_chart_size_get(chart, &w, &h);

    c = enesim_renderer_compound_new();

    bg = enesim_renderer_rectangle_new();
    enesim_renderer_rectangle_position_set(bg, 0, 0);
    enesim_renderer_rectangle_size_set(bg, w, h);
    enesim_renderer_shape_fill_color_set(bg, echart_chart_background_color_get(chart));
    enesim_renderer_shape_draw_mode_set(bg, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);

    l = enesim_renderer_compound_layer_new();
    enesim_renderer_compound_layer_renderer_set(l, bg);
    enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_FILL);
    enesim_renderer_compound_layer_add(c, l);

    echart_chart_grid_nbr_get(chart, &grid_x_nbr, &grid_y_nbr);
    for (i = 0; i < grid_x_nbr; i++)
    {
        r = enesim_renderer_line_new();
        enesim_renderer_line_coords_set(r, (i * w) / (double)grid_x_nbr, 0, (i * w) / (double)grid_x_nbr, h);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_stroke_color_set(r, 0xff000000);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    for (i = 0; i < grid_y_nbr; i++)
    {
        r = enesim_renderer_line_new();
        enesim_renderer_line_coords_set(r, 0, (i * h) / (double)grid_y_nbr, w, (i * h) / (double)grid_y_nbr);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_stroke_color_set(r, 0xff000000);
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    data = echart_chart_data_get(chart);
    absciss = echart_data_items_get(data, 0);
    echart_data_item_interval_get(absciss, &avmin, &avmax);

    for (j = 1; j < echart_data_items_count(data); j++)
    {
        double vmin;
        double vmax;

        item = echart_data_items_get(data, j);
        echart_data_item_interval_get(item, &vmin, &vmax);

        p = enesim_path_new();
        for (i = 0; i < eina_list_count(echart_data_item_values_get(item)); i++)
        {
            double d1;
            double d2;

            d1 = *(double *)eina_list_nth(echart_data_item_values_get(absciss), i);
            d1 = w * (d1 - avmin) / (avmax - avmin);
            d2 = *(double *)eina_list_nth(echart_data_item_values_get(item), i);
            d2 = h * (d2 - vmin) / (vmax - vmin);
            if (i == 0)
                enesim_path_move_to(p, d1, h - d2);
            else
                enesim_path_line_to(p, d1, h - d2);
        }

        r = enesim_renderer_path_new();
        enesim_renderer_path_path_set(r, p);
        enesim_renderer_shape_stroke_weight_set(r, 1);
        enesim_renderer_shape_stroke_color_set(r, echart_data_item_color_get(item));
        enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE);

        l = enesim_renderer_compound_layer_new();
        enesim_renderer_compound_layer_renderer_set(l, r);
        enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
        enesim_renderer_compound_layer_add(c, l);
    }

    return c;
}