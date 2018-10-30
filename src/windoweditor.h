/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#ifndef INCLUDED_WINDOW_EDITOR_H
#define INCLUDED_WINDOW_EDITOR_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "floatingwindow.h"
#include "usfmview.h"
#include "editor.h"
#include "editor3.h"

class WindowEditor : public FloatingWindow
{
public:
  WindowEditor(const ustring& project_name, GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup, viewType vt);
  virtual ~WindowEditor();
 private:
  void init(void);
  void cleanup(void);
  
  ustring projectname;
  viewType currvt;
 public:
  inline ustring projectname_get(void) { return projectname; }
  inline viewType vt_get() { return currvt; }
  void vt_set(viewType newvt);

  void go_to(const Reference& reference);
  Reference current_reference();
  ustring current_verse_number();
  vector <Reference> quick_references();
  void go_to_new_reference_highlight_set();
  
  void load_dictionaries();
  bool move_cursor_to_spelling_error (bool next, bool extremity);
  vector <ustring> spelling_get_misspelled ();
  void spelling_approve (const vector <ustring>& words);
  
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();
  ustring text_get_selection();
  void insert_text(const ustring &text);
  ustring word_double_clicked_text();
  void insert_note(const ustring& marker, const ustring& rawtext);
  ustring get_chapter();
  void insert_table(const ustring& rawtext);
  void chapter_load(const Reference &ref);
  void chapter_save();
  unsigned int reload_chapter_number();
  
  EditorTextViewType last_focused_type();

  ustring project();
  bool editable();
  unsigned int book();
  unsigned int chapter();

  void apply_style(const ustring& marker);
  set <ustring> get_styles_at_cursor();
  void create_or_update_formatting_data();
  void set_font();

  Editor2 * editor_get();

  // The signalling instance                The handler_id
  GtkWidget * new_verse_signal;             gulong hID1;
  GtkWidget * new_styles_signal;            gulong hID2;
  GtkWidget * quick_references_button;      gulong hID3;
  GtkWidget * word_double_clicked_signal;   gulong hID4;
  GtkWidget * reload_signal;                gulong hID5;
  GtkWidget * changed_signal;               gulong hID6;
  GtkWidget * spelling_checked_signal;      gulong hID7;

  // stray?
  gulong hID8;
  
  GtkTextBuffer * edit_usfm_textbuffer ();

  void spelling_trigger ();
    
  void cut ();
  void copy ();
  void paste ();

protected:
  GtkWidget *vbox;
  void switch_view ();
  ChapterView * currView; // this is either NULL or equal to one of the following editor/view pointers
  Editor2 * editor2;
  USFMView * usfmview;
  Editor3 * editor3; // experimental editor goes here: no-paragraph-division view

  static void on_new_verse_signalled(GtkButton *button, gpointer user_data);
  void on_new_verse();
  static void on_new_styles_signalled(GtkButton *button, gpointer user_data);
  void on_new_styles();
  static void on_quick_references_signalled(GtkButton *button, gpointer user_data);
  void on_quick_references();
  static void on_word_double_click_signalled(GtkButton *button, gpointer user_data);
  void on_word_double_click();
  static void on_reload_signalled(GtkButton *button, gpointer user_data);
  void on_reload();
  static void on_changed_signalled(GtkButton *button, gpointer user_data);
  void on_changed();
  static void on_spelling_checked_signalled(GtkButton *button, gpointer user_data);
  void on_spelling_checked();
  static void on_new_widget_signal_clicked(GtkButton *button, gpointer user_data);
  void on_new_widget_signal ();
  
private:
};


#endif
