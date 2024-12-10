#include <gtk/gtk.h>

// Callback function to show the name entered in the entry widget
static void show_name(GtkWidget *widget, gpointer data) {
    const char *name = gtk_entry_get_tabs(GTK_ENTRY(data));  // Get text from entry widget
    GtkWidget *label = gtk_label_new(NULL);  // Create a new label widget
    gchar *message = g_strdup_printf("Your name is: %s", name);  // Create message with entered name
    gtk_label_set_text(GTK_LABEL(label), message);  // Set the label's text
    gtk_widget_show(label);  // Show the label

    // Attach the label to the window
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_widget_show_all(window);  // Show all widgets in the window
    g_free(message);  // Free the allocated memory for the message
}

// Callback function to add entry and submit button to the window
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);  // Get the window from the callback data
    GtkWidget *entry = gtk_entry_new();  // Create a new entry widget
    gtk_widget_set_size_request(entry, 200, -1);  // Set size for the entry widget
    gtk_widget_show(entry);  // Show the entry widget

    // Create a new button to submit the name
    GtkWidget *button2 = gtk_button_new_with_label("Submit");
    gtk_widget_set_size_request(button2, 100, 30);  // Set size for the submit button
    gtk_widget_show(button2);  // Show the submit button

    // Attach entry and button to the window
    gtk_container_add(GTK_CONTAINER(window), entry);
    gtk_container_add(GTK_CONTAINER(window), button2);

    // Connect the submit button to the show_name function
    g_signal_connect(button2, "clicked", G_CALLBACK(show_name), entry);

    gtk_widget_show_all(window);  // Show all widgets in the window
}

// Main function to initialize GTK and create the window with a button
int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_button_clicked), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
