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


#ifndef INCLUDED_EDITOR3_H
#define INCLUDED_EDITOR3_H

//enum EditorMovementType {emtForward, emtBack, emtUp, emtDown};

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "style.h"
#include "types.h"
#include "libraries.h"
#include "highlight.h"
#include "spelling.h"
#include "chapterview.h"

class Editor3 : public ChapterView // Editor3 has to implement the ChapterView interface
{
private:
    enum EditorActionType {
        eatCreateParagraph,
        eatChangeParagraphStyle,
        eatInsertText,
        eatDeleteText,
        eatChangeCharacterStyle,
        eatLoadChapterBoundary,
        eatOneActionBoundary,
        eatDeleteParagraph,
        eatCreateNoteParagraph
    };
    
    
    enum EditorActionApplication {
        eaaInitial,
        eaaUndo,
        eaaRedo
    };
    
    class EditorAction
    {
    public:
        EditorAction(Editor3 *_parent_editor, EditorActionType type_in);
        virtual ~EditorAction();
        void apply (deque <EditorAction *>& done);
        void undo (deque <EditorAction *>& done, deque <EditorAction *>& undone);
        void redo (deque <EditorAction *>& done, deque <EditorAction *>& undone);
        EditorActionType type;
    private:
    protected:
        Editor3 *parent_editor;
    };
    
    // Fwd decls
    class EditorActionCreateNoteParagraph;
    class EditorActionDeleteParagraph;
    
    class EditorActionCreateParagraph : public EditorAction
    {
    public:
        EditorActionCreateParagraph(Editor3 *_parent_editor, GtkWidget * vbox);
        virtual ~EditorActionCreateParagraph();
        friend class EditorActionCreateNoteParagraph;
        friend class EditorActionDeleteParagraph;
        void apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus);
        void undo (GtkWidget * parking_vbox, GtkWidget *& to_focus);
        void redo (GtkWidget *& to_focus);
        GtkWidget * textview;
        GtkTextBuffer * textbuffer;
        ustring style;
    private:
        gint offset_at_delete;
        GtkWidget * parent_vbox;
    };
    
    
    class EditorActionChangeParagraphStyle : public EditorAction
    {
    public:
        EditorActionChangeParagraphStyle(Editor3 *_parent_editor, const ustring& style, EditorActionCreateParagraph * parent_action);
        virtual ~EditorActionChangeParagraphStyle();
        void apply (GtkWidget *& to_focus);
        void undo (GtkWidget *& to_focus);
        void redo (GtkWidget *& to_focus);
    private:
        EditorActionCreateParagraph * paragraph;
        ustring previous_style;
        ustring current_style;
        void set_style (const ustring& style);
    };
    
    
    class EditorActionInsertText : public EditorAction
    {
    public:
        EditorActionInsertText(Editor3 *_parent_editor, EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in);
        virtual ~EditorActionInsertText();
        void apply (GtkWidget *& to_focus);
        void undo (GtkWidget *& to_focus);
        void redo (GtkWidget *& to_focus);
    private:
        EditorActionCreateParagraph * paragraph;
        gint offset;
        ustring text;
    };
    
    
    class EditorActionDeleteText : public EditorAction
    {
    public:
        EditorActionDeleteText(Editor3 *_parent_editor, EditorActionCreateParagraph * parent_action, gint offset_in, gint length_in);
        virtual ~EditorActionDeleteText();
        EditorActionCreateParagraph * paragraph;
        void apply (GtkWidget *& to_focus);
        void undo (GtkWidget *& to_focus);
        void redo (GtkWidget *& to_focus);
    private:
        gint offset;
        gint length;
        vector <ustring> deleted_text;
        vector <ustring> deleted_styles;
    };
    
    
    class EditorActionChangeCharacterStyle : public EditorAction
    {
    public:
        EditorActionChangeCharacterStyle(Editor3 *_parent_editor, EditorActionCreateParagraph * parent_action, const ustring& style_in, gint offset_in, gint length_in);
        virtual ~EditorActionChangeCharacterStyle();
        void apply (GtkWidget *& to_focus);
        void undo (GtkWidget *& to_focus);
        void redo (GtkWidget *& to_focus);
    private:
        EditorActionCreateParagraph * paragraph;
        ustring style;
        gint offset;
        gint length;
        vector <ustring> previous_styles;
        void change_styles (const vector <ustring>& old_ones, const vector <ustring>& new_ones); 
    };


    class EditorActionDeleteParagraph : public EditorAction
    {
    public:
        EditorActionDeleteParagraph(Editor3 *_parent_editor, EditorActionCreateParagraph * paragraph_in);
        virtual ~EditorActionDeleteParagraph();
        void apply(GtkWidget * parking_vbox, GtkWidget *& to_focus);
        void undo (GtkWidget *& to_focus);
        void redo (GtkWidget * parking_vbox, GtkWidget *& to_focus);
    private:
        EditorActionCreateParagraph * paragraph;
        gint offset;
    };
    
    
    class EditorActionCreateNoteParagraph : public EditorActionCreateParagraph
    {
    public:
        EditorActionCreateNoteParagraph(Editor3 *_parent_editor, GtkWidget * vbox, const ustring& marker_in, const ustring& caller_usfm_in, const ustring& caller_text_in, const ustring& identifier_in);
        virtual ~EditorActionCreateNoteParagraph();
        void apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus);
        void undo  (GtkWidget * parking_vbox, GtkWidget *& to_focus);
        void redo  (GtkWidget *& to_focus);
        GtkWidget * hbox;
        GtkWidget * eventbox;
        ustring identifier;
        ustring opening_closing_marker;
        ustring caller_usfm;
    private:
        ustring caller_text;
        GtkWidget * label;
        static gboolean on_caller_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
        gboolean on_caller_enter_notify (GdkEventCrossing *event);
        static gboolean on_caller_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
        gboolean on_caller_leave_notify (GdkEventCrossing *event);
    };
    
    
public:
  Editor3(GtkWidget * vbox_in, const ustring& project_in);
  ~Editor3();
  viewType vt_get() { return vtFormatted; }
  GtkWidget * new_widget_signal;
  GtkWidget * new_widget_pointer;
  GtkWidget *scrolledwindow;
private:
  GtkWidget *viewport;
  GtkWidget *vbox_viewport;
  
  // Where the text of the Bible is shown
  GtkWidget *vbox_paragraphs;
  GtkWidget *textview;
  GtkTextBuffer *textbuffer;

  GtkWidget *hseparator;

  // Where the footnotes/endnotes/cross-refs are shown
  GtkWidget *vbox_notes;
  GtkWidget *notetextview;
  GtkTextBuffer *notetextbuffer;
  
  // Where the undo information is stored
  GtkWidget *vbox_parking_lot;
  
  deque <EditorAction *> actions_done;
  deque <EditorAction *> actions_undone;
  void apply_editor_action (EditorAction * action, EditorActionApplication application = eaaInitial);
#ifdef OLD_STUFF
  void paragraph_create_actions (EditorActionCreateParagraph * paragraph_action);
#endif
  void textviewbuffer_create_actions (GtkTextBuffer *textbuffer, GtkWidget *textview);
public:
  //EditorActionCreateParagraph * focused_paragraph;
  GtkTextView *focused_textview; // pointer to either textview or notetextview
private:
  bool text_starts_paragraph (ustring& line, StyleType type, int subtype, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool text_starts_verse (ustring& line, StyleType type, int subtype, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  void gtk_text_buffer_append_with_markers (GtkTextBuffer *textbuffer, const ustring& text, GtkTextIter &startiter, GtkTextIter &enditer);
  void gtk_text_buffer_append (GtkTextBuffer *textbuffer, const ustring& text);
  GtkWidget *create_text_view(GtkWidget *parent_vbox);
  void text_load (ustring text, ustring character_style, bool note_mode);
  
  void editor_start_new_standard_paragraph (const ustring& marker_text);
  bool editor_starts_character_style (ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool editor_ends_character_style   (ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool text_starts_note_raw          (ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found, ustring& raw_note);
  void get_next_note_caller_and_style (EditorNoteType type, ustring& caller, ustring& style, bool restart);
  unsigned int note_style_num;
  EditorActionCreateParagraph * widget2paragraph_action (GtkWidget * widget);
  EditorActionCreateNoteParagraph * note2paragraph_action (const ustring& note);
  int disregard_text_buffer_signals;
  // These are filled every time a piece of text is deleted from the textview in buffer_delete_range_before
  vector <ustring> text_to_be_deleted;
  vector <ustring> styles_to_be_deleted;
  ustring usfm_get_text(GtkTextBuffer * textbuffer, GtkTextIter startiter, GtkTextIter enditer);

  // Textview keyboard key pressing.
  static gboolean on_textview_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  gboolean textview_key_press_event(GtkWidget *widget, GdkEventKey *event);
  unsigned int keystrokeNum;
  unsigned int keyStrokeNum_paragraph_crossing_processed;
  // Next two obsolete. We should NOT be processing things when the user releases a key, after holding
  // it down for an indeterminate length of time (like backspace or delete in particular)
  //static gboolean on_textview_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  //void textview_key_release_event(GtkWidget *widget, GdkEventKey *event);
  bool textbuffer_delete_range_was_fired;
  static void on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data);
  void textbuffer_changed(GtkTextBuffer * textbuffer);

  // Textview mouse button pressing signal and delay.
  static gboolean on_textview_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean textview_button_press_event(GtkWidget * widget, GdkEventButton *event);
  guint textview_button_press_event_id;
  static bool on_textview_button_press_delayed (gpointer user_data);
  void textview_button_press_delayed ();

  // Textview focus grabbing signal and delay.
  static void on_textview_grab_focus(GtkWidget * widget, gpointer user_data);
  void textview_grab_focus(GtkWidget * widget);
  guint textview_grab_focus_event_id;
  static bool on_textview_grab_focus_delayed(gpointer data);
  void textview_grab_focus_delayed();

  // Textview cursor movement signal and delay.
  static void on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data);
  void textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count);
  guint textview_move_cursor_id;
  static bool on_textview_move_cursor_delayed(gpointer user_data);
  void textview_move_cursor_delayed();

  // Focus handling.
public:
  GtkWidget * last_focused_widget;
  GtkTextBuffer * last_focused_textbuffer();
  EditorTextViewType last_focused_type();
  bool has_focus ();
  void give_focus (GtkWidget * widget);

  ustring character_style_on_start_typing;

  // Chapter loading / saving.
  void chapter_load(const Reference &ref);
  vector <ustring> loaded_chapter_lines;
  void chapter_save();
  GtkWidget * reload_signal;
 private:
  unsigned int reload_chapter_number;
 public:
  unsigned int reload_chapter_num_get() { return reload_chapter_number; }
  ustring text_get_selection();
  void insert_text(const ustring &text);
  void insert_table(const ustring& rawtext);

  void show_quick_references();
  guint event_id_show_quick_references;
  static bool show_quick_references_timeout(gpointer user_data);
  void show_quick_references_execute();
 private:
  vector <Reference> quick_references;
  ustring word_double_clicked_text;
 public:
  vector<Reference> quick_references_get() { return quick_references; }
  GtkWidget * quick_references_button;
  GtkWidget * new_styles_signal;
  bool go_to_new_reference_highlight;
  ustring word_double_clicked_text_get() { return word_double_clicked_text; }
  GtkWidget * word_double_clicked_signal;

  void font_set();
  void set_font_textview (GtkWidget * textview);
 private:
  ustring project;
 public:
  ustring project_get() { return project; }
  set <ustring> get_styles_at_cursor();

  // Automatic save.
  static bool on_save_timeout(gpointer data);
  bool save_timeout();
  size_t editor_actions_size_at_no_save;

  // Some event ids.
  guint save_timeout_event_id;

  GtkTextTagTable * texttagtable;
  void create_or_update_formatting_data();
  void create_or_update_text_style(Style * style, bool paragraph, bool plaintext, double font_multiplier);
  bool verse_restarts_paragraph;
  double font_size_multiplier;
  
  void insert_note(const ustring& marker, const ustring& rawtext);

  bool do_not_process_child_anchors_being_deleted;
  static void on_buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data);
  void buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text);
  static void on_buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  static void on_buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  void text_get_all(vector <ustring>& texts, vector <VectorUstring>& styles);

 private:
  bool editable;
 public:
  bool editable_get() { return editable; }

  void signal_if_styles_changed();
  set <ustring> styles_at_cursor;
  set <ustring> styles_at_iterator(GtkTextIter iter);
  void apply_style(const ustring& marker);

  // Undo/redo
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();

  // Highlighting (search results, not verse numbers)
  void highlight_searchwords();
  guint highlight_timeout_event_id;
  static bool on_highlight_timeout(gpointer data);
  bool highlight_timeout();
  static void highlight_thread_start(gpointer data);
  void highlight_thread_main();
  Highlight * highlight;
  GtkTextTag * reference_tag;

  GtkWidget * changed_signal;
  ustring chapter_get_ustring();
private:

  // Spelling check.
public:
  void load_dictionaries();
  bool move_cursor_to_spelling_error (bool next, bool extremity);
  GtkWidget * spelling_checked_signal;
  void spelling_trigger();
  vector <ustring> spelling_get_misspelled ();
  void spelling_approve (const vector <ustring>& words);
private:
  guint spelling_timeout_event_id;
  static bool on_spelling_timeout(gpointer data);
  void spelling_timeout();
  SpellingChecker * spellingchecker;
  static void on_button_spelling_recheck_clicked(GtkButton *button, gpointer user_data);

  // Verse positioning and tracking.
private:
  ustring current_verse_number;
public:
  void go_to_verse(const ustring& versenum, bool focus=false);
  GtkWidget * new_verse_signal;
  ustring current_verse_get() { return current_verse_number; }
  void switch_verse_tracking_off ();
  void switch_verse_tracking_on ();
  bool verse_tracking_on;
  void signal_if_verse_changed ();
  guint signal_if_verse_changed_event_id;
  static bool on_signal_if_verse_changed_timeout(gpointer data);
  void signal_if_verse_changed_timeout();
  ustring verse_number_get();
  ustring get_verse_number_at_iterator(GtkTextIter iter, const ustring & verse_marker, const ustring & project);
  bool    get_iterator_at_verse_number (const ustring& verse_number, const ustring& verse_marker, GtkTextIter & iter, GtkWidget *& textview, bool deep_search = false);

  // Scrolling control and verse highlighting.
private:
  void scroll_to_insertion_point_on_screen(GtkWidget *textview);
  void highlightCurrVerse(GtkWidget *textview);
  ustring currHighlightedVerse;
  GtkTextTag * verse_highlight_tag;
  
  // Moving from one textview to the other.
private:
#ifdef OLDSTUFF
  void paragraph_crossing_act(GtkMovementStep step, gint count);
#endif
  GtkWidget * paragraph_crossing_textview_at_key_press;
  GtkTextIter paragraph_crossing_insertion_point_iterator_at_key_press;
  void combine_paragraphs(EditorActionCreateParagraph * first_paragraph, EditorActionCreateParagraph * second_paragraph);
  
  // Clipboard.
public:
  void cut ();
  void copy ();
  void paste ();
private:
  void copy_clipboard_intelligently ();
  ustring clipboard_text_plain;
  ustring clipboard_text_usfm;

  // Cursor control.
public:
private:
  static gboolean on_motion_notify_event(GtkWidget *textview, GdkEventMotion *event, gpointer user_data);
  gboolean motion_notify_event(GtkWidget *textview, GdkEventMotion *event);
  bool previous_hand_cursor;


  static gboolean on_caller_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_caller_button_press (GtkWidget *widget);

public:
  EditorNoteType note_type_get(const ustring& project, const ustring& marker);
private:
  NoteNumberingType note_numbering_type_get(const ustring& project, const ustring& marker);
  ustring note_numbering_user_sequence_get(const ustring& project, const ustring& marker);
  
  gint table_get_n_rows(GtkTable * table);
  gint table_get_n_columns(GtkTable * table);
  
  void usfm_internal_add_text(ustring& text, const ustring& addition);
  void usfm_internal_get_text_close_character_style(ustring& text, const ustring& project, const ustring& style);
  ustring usfm_get_note_text(GtkTextIter startiter, GtkTextIter enditer, const ustring& project);
public:  
  vector <ustring> get_character_styles_between_iterators (GtkTextIter startiter, GtkTextIter enditer);
  void textbuffer_apply_named_tag(GtkTextBuffer *buffer, const ustring& name, const GtkTextIter *start, const GtkTextIter *end);
private:
  void textbuffer_insert_with_named_tags(GtkTextBuffer *buffer, GtkTextIter *iter, const ustring& text, ustring first_tag_name, ustring second_tag_name);
  
  void clear_and_destroy_editor_actions (deque <EditorAction *>& actions);
  static void on_container_tree_callback_destroy (GtkWidget *widget, gpointer user_data);
  void editor_text_append(GtkTextBuffer * textbuffer, const ustring & text, const ustring & paragraph_style, const ustring & character_style);
  
  gint editor_paragraph_insertion_point_get_offset (EditorActionCreateParagraph * paragraph_action);
  void editor_paragraph_insertion_point_set_offset (EditorActionCreateParagraph * paragraph_action, gint offset);
  
  EditorActionDeleteText * paragraph_delete_last_character_if_space(EditorActionCreateParagraph * paragraph_action);
  EditorActionDeleteText * paragraph_get_text_and_styles_after_insertion_point(EditorActionCreateParagraph * paragraph, vector <ustring>& text, vector <ustring>& styles);
public:  
  void get_text_and_styles_between_iterators(GtkTextIter * startiter, GtkTextIter * enditer, vector <ustring>& text, vector <ustring>& styles);

  typedef pair<vector<GtkWidget *> *, GType> widget_search_t;
  
public:
  void editor_park_widget (GtkWidget * vbox, GtkWidget * widget, gint& offset, GtkWidget * parking);
};

#endif
