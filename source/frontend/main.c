#include "data.h"
#include "version.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// declarations

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *buttonEnc;
GtkWidget *buttonDec;
GtkEntry *inputEnc;
GtkEntry *inputPlain;
GtkLabel *labelStatus;
GtkWidget *inputKey;

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    if (0 == strcmp(argv[1], "--version"))
      version();
  }

  gtk_init(&argc, &argv);

  builder = gtk_builder_new_from_resource(APP_PREFIX "/window_main.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

  gtk_builder_connect_signals(builder, NULL);
  // g_object_unref(builder);
  // bindings
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  buttonEnc = GTK_WIDGET(gtk_builder_get_object(builder, "buttonEnc"));
  buttonDec = GTK_WIDGET(gtk_builder_get_object(builder, "buttonDec"));
  inputEnc = GTK_ENTRY(gtk_builder_get_object(builder, "inputEnc"));
  inputPlain = GTK_ENTRY(gtk_builder_get_object(builder, "inputPlain"));
  labelStatus = GTK_LABEL(gtk_builder_get_object(builder, "labelStatus"));
  inputKey = GTK_WIDGET(gtk_builder_get_object(builder, "inputKey"));

  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}

gint getKey() {
  return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(inputKey));
}
const gchar *getPlain() { return gtk_entry_get_text(GTK_ENTRY(inputPlain)); }
gint *getPlainL() { return gtk_entry_get_text_length(GTK_ENTRY(inputPlain)); }
const gchar *getEnc() { return gtk_entry_get_text(GTK_ENTRY(inputEnc)); }
gint *getEncL() { return gtk_entry_get_text_length(GTK_ENTRY(inputEnc)); }

char *esc_special(const char* input, bool isDec) {

  size_t length;
  if(isDec){
    length = (size_t)getEncL();
  } else {
    length = (size_t)getPlainL();
  }
  

  char* out = (char*)malloc(2 * length + 1);
      /*Allocate 2x the length as its is theoritical maximum of escape
       characters possible and add one for NULL terminator
       */


      if (out == NULL) {
    perror("malloc");
    return NULL;
  }

  size_t out_index = 0; // Will come usefull with adding null at end
  

  for (size_t i = 0; i < length; ++i) {
    if (input[i] == '\\') {
      out[out_index++] = '\\';
      out[out_index++] = '\\';
    } else if (input[i] == '\"') {
      out[out_index++] = '\\';
      out[out_index++] = '\"';
    } else {
      out[out_index++] = input[i];
    }
  }

  out[out_index] = '\0';
  return out;
}

char *fetchEnc(const gchar *plain, gint key, bool isDec) {
  char baseCommand[15] = "caesar-cli enc";

  if (isDec) {
    baseCommand[11] = 'd';
    baseCommand[12] = 'e';
  }


  char* inputCopy = strdup(plain);
  char* escapedInput = esc_special(inputCopy, isDec);
  

  char fullCommand[512];
  snprintf(fullCommand, sizeof(fullCommand), "%s %d \"%s\" | cut -c9-",
           baseCommand, key, escapedInput);

  printf("%s\n", fullCommand);

  FILE *pipe = popen(fullCommand, "r");
  if (pipe == NULL) {
    perror("popen");
    return NULL;
  }

  size_t buffer_size = 128;
  char *buffer = (char *)malloc(buffer_size);
  if (buffer == NULL) {
    perror("malloc");
    return NULL;
  }

  char *result = NULL;
  size_t result_size = 0;

  while (fgets(buffer, buffer_size, pipe) != NULL) {
    size_t len = strlen(buffer);

    const char *new_result = (char *)realloc(result, result_size + len + 1);
    if (new_result == NULL) {
      perror("realloc");
      free(result);
      free(buffer);
      return NULL;
    }
    result = new_result;
    strcpy(result + result_size, buffer); // Append the data
    result_size += len;
  }

  pclose(pipe);

  result[result_size-1] = '\0';
  printf("%s\n", result);
  return result;
}

void on_buttonEnc_clicked(GtkButton *button) {
  printf("%d\n", getPlainL());
  if (getPlainL() == 0) {
    gtk_label_set_text(GTK_ENTRY(labelStatus),
                       (const gchar *)"Plain input is empty!");
  } else {
    const gchar *enc_out = fetchEnc(getPlain(), getKey(), false);

    if (enc_out != NULL) {
      gtk_entry_set_text(GTK_ENTRY(inputEnc), enc_out);
      gtk_label_set_text(GTK_ENTRY(labelStatus), (const gchar *)"Message encrypted");
      free(enc_out);
    }
  }
}

void on_buttonDec_clicked(GtkButton *button) {
  if (getEncL() == 0) {
    gtk_label_set_text(GTK_ENTRY(labelStatus),
                       (const gchar *)"Ecrypted input is empty!");
  } else {
    const gchar *dec_out = fetchEnc(getEnc(), getKey(), true);

    if (dec_out != NULL) {
      gtk_entry_set_text(GTK_ENTRY(inputPlain), dec_out);
      gtk_label_set_text(GTK_ENTRY(labelStatus), (const gchar *)"Message decrypted");
      free(dec_out);
    }
  }
}

void on_window_main_destroy() { gtk_main_quit(); }
