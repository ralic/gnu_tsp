/* gtkplotdata - 2d scientific plots widget for gtk+
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GTK_PLOT_DATA_H__
#define __GTK_PLOT_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"

#define GTK_PLOT_DATA(obj)        GTK_CHECK_CAST (obj, gtk_plot_data_get_type (), GtkPlotData)
#define GTK_TYPE_PLOT_DATA   (gtk_plot_data_get_type ())
#define GTK_PLOT_DATA_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_data_get_type(), GtkPlotDataClass)
#define GTK_IS_PLOT_DATA(obj)     GTK_CHECK_TYPE (obj, gtk_plot_data_get_type ())


GtkType		gtk_plot_data_get_type		(void);
GtkWidget*	gtk_plot_data_new		(void);
GtkWidget* 	gtk_plot_data_new_function 	(GtkPlotFunc function);
void		gtk_plot_data_construct_function (GtkPlotData *data,
						  GtkPlotFunc function);
GtkWidget* 	gtk_plot_data_new_iterator 	(GtkPlotIterator iter,	
						 gint npoints,
						 guint16 iterator_mask);
void		gtk_plot_data_construct_iterator (GtkPlotData *data,
						  GtkPlotIterator iter,
						  gint npoints,
						  guint16 iterator_mask);
/* draw last n points */
void 		gtk_plot_data_paint		(GtkPlotData *data);
void 		gtk_plot_data_update		(GtkPlotData *data);
void 		gtk_plot_data_draw_points	(GtkPlotData *data,
						 gint n);
void 		gtk_plot_data_draw_symbol	(GtkPlotData *data, 
                                                 gdouble x, gdouble y); 
void 		gtk_plot_data_set_points	(GtkPlotData *data,
						 gdouble *x, gdouble *y,
						 gdouble *dx, gdouble *dy,
                                                 gint num_points);
void 		gtk_plot_data_get_points	(GtkPlotData *data,
						 gdouble **x, gdouble **y,
						 gdouble **dx, gdouble **dy,
                                                 gint *num_points);
void 		gtk_plot_data_get_point		(GtkPlotData *data,
						 gint n,
						 gdouble *x, gdouble *y,
						 gdouble *z, gdouble *a,
						 gdouble *dx, gdouble *dy,
						 gdouble *dz, gdouble *da,
						 gchar **label,
                                                 gboolean *error);
void 		gtk_plot_data_set_x		(GtkPlotData *data,
						 gdouble *x); 
void 		gtk_plot_data_set_y		(GtkPlotData *data,
						 gdouble *y); 
void 		gtk_plot_data_set_z		(GtkPlotData *data,
						 gdouble *z); 
void 		gtk_plot_data_set_a		(GtkPlotData *data,
						 gdouble *z); 
void 		gtk_plot_data_set_dx		(GtkPlotData *data,
						 gdouble *dx); 
void 		gtk_plot_data_set_dy		(GtkPlotData *data,
						 gdouble *dy); 
void 		gtk_plot_data_set_dz		(GtkPlotData *data,
						 gdouble *dz); 
void 		gtk_plot_data_set_da		(GtkPlotData *data,
						 gdouble *da); 
void 		gtk_plot_data_set_a_scale	(GtkPlotData *data,
						 gdouble a_scale); 
void 		gtk_plot_data_set_labels	(GtkPlotData *data,
						 gchar **labels); 
gdouble * 	gtk_plot_data_get_x		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_y		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_z		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_a		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_dx		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_dy		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_dz		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble * 	gtk_plot_data_get_da		(GtkPlotData *data, 
                                                 gint *num_points);
gdouble 	gtk_plot_data_get_a_scale	(GtkPlotData *data);
gchar ** 	gtk_plot_data_get_labels	(GtkPlotData *data,
                                                 gboolean *show_labels);
void    	gtk_plot_data_show_labels	(GtkPlotData *data,
                                                 gboolean show_labels);
void		gtk_plot_data_labels_set_attributes	(GtkPlotData *data,
							 const gchar *font,
							 gint height,
							 gint angle,
							 const GdkColor *foreground,
							 const GdkColor *background);
void		gtk_plot_data_set_numpoints  	(GtkPlotData *data,
                                                 gint num_points);
gint		gtk_plot_data_get_numpoints  	(GtkPlotData *data);
void		gtk_plot_data_set_symbol     	(GtkPlotData *data,
                                                 GtkPlotSymbolType type,
                                                 GtkPlotSymbolStyle style,
						 gint size,
						 gfloat line_width,
						 const GdkColor *color,
						 const GdkColor *border_color);
void		gtk_plot_data_get_symbol   	(GtkPlotData *data,
                                                 GtkPlotSymbolType *type,
                                                 GtkPlotSymbolStyle *style,
						 gint *size,
						 gfloat *line_width,
						 GdkColor *color,
						 GdkColor *border_color);
void		gtk_plot_data_set_connector     (GtkPlotData *data,
						 GtkPlotConnector connector); 
gint		gtk_plot_data_get_connector     (GtkPlotData *data);
void		gtk_plot_data_set_line_attributes 	(GtkPlotData *data,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_data_get_line_attributes 	(GtkPlotData *data,
						 	 GtkPlotLineStyle *style,
						 	 gfloat *width,
						 	 GdkColor *color);
void		gtk_plot_data_set_x_attributes 		(GtkPlotData *data,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_data_set_y_attributes 		(GtkPlotData *data,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_data_set_z_attributes 		(GtkPlotData *data,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_data_show_xerrbars  		(GtkPlotData *data);
void		gtk_plot_data_show_yerrbars 	 	(GtkPlotData *data);
void		gtk_plot_data_show_zerrbars 	 	(GtkPlotData *data);
void		gtk_plot_data_hide_xerrbars  		(GtkPlotData *data);
void		gtk_plot_data_hide_yerrbars  		(GtkPlotData *data);
void		gtk_plot_data_hide_zerrbars  		(GtkPlotData *data);
void    	gtk_plot_data_fill_area			(GtkPlotData *data,
                                                 	 gboolean fill);
gboolean    	gtk_plot_data_area_is_filled		(GtkPlotData *data);
void 		gtk_plot_data_show_legend 		(GtkPlotData *data);
void 		gtk_plot_data_hide_legend 		(GtkPlotData *data);
void            gtk_plot_data_set_legend		(GtkPlotData *dataset,
                            				 const gchar *legend);
void            gtk_plot_data_set_legend_precision	(GtkPlotData *dataset,
                            				 gint precision);
gint            gtk_plot_data_get_legend_precision	(GtkPlotData *dataset);
void		gtk_plot_data_set_name       		(GtkPlotData *data,
                                                	 const gchar *name);
void		gtk_plot_data_set_gradient_mask		(GtkPlotData *data,
							 gint mask);
gint		gtk_plot_data_get_gradient_mask		(GtkPlotData *data);
void		gtk_plot_data_gradient_set_visible	(GtkPlotData *data,
							 gboolean visible);
gboolean	gtk_plot_data_gradient_visible		(GtkPlotData *data);
void		gtk_plot_data_draw_gradient		(GtkPlotData *data,
							 gint x, gint y);
void		gtk_plot_data_gradient_autoscale_a	(GtkPlotData *data);
void		gtk_plot_data_gradient_autoscale_da	(GtkPlotData *data);
void		gtk_plot_data_gradient_autoscale_z	(GtkPlotData *data);
void		gtk_plot_data_set_gradient_colors	(GtkPlotData *data,
							 const GdkColor *min,
							 const GdkColor *max);
void		gtk_plot_data_get_gradient_colors	(GtkPlotData *data,
							 GdkColor *min,
							 GdkColor *max);
void		gtk_plot_data_set_gradient		(GtkPlotData *data,
							 gdouble min,
							 gdouble max,
							 gint nlevels,
							 gint nsublevels);
void		gtk_plot_data_get_gradient		(GtkPlotData *data,
							 gdouble *min,
							 gdouble *max,
							 gint *nlevels,
							 gint *nsublevels);
void 		gtk_plot_data_get_gradient_level 	(GtkPlotData *data,
							 gdouble level,
							 GdkColor *color);
void		gtk_plot_data_set_link			(GtkPlotData *data,
							 gpointer link);
gpointer	gtk_plot_data_get_link			(GtkPlotData *data);
void		gtk_plot_data_remove_link		(GtkPlotData *data);

/* Markers */

GtkPlotMarker *	gtk_plot_data_add_marker		(GtkPlotData *data,
							 guint point);
gboolean	gtk_plot_data_remove_marker		(GtkPlotData *data,
							 GtkPlotMarker *marker);
void		gtk_plot_data_remove_markers		(GtkPlotData *data);
void		gtk_plot_data_show_markers		(GtkPlotData *data,
							 gboolean show);
gboolean	gtk_plot_data_markers_visible		(GtkPlotData *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_DATA_H__ */
