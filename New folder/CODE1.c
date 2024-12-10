#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Callback function for button click events
static void print_hello(GtkWidget *widget, gpointer data) {
    g_print("Hello World\n");
}


typedef struct DateTime {
    int day, month, year, hour, minute;
} DATETIME;

typedef struct properties {
    char name[50];
    char date[20]; // Store the formatted date as a string
    char size[50];
} Properties;


// Function to get the current date and time as a formatted string
char* get_formatted_date() {
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    char *formatted_date = (char *)malloc(20 * sizeof(char));
    if (formatted_date == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    snprintf(formatted_date, 20, "%02d/%02d/%04d-%02d:%02d",
             local_time->tm_mday,
             local_time->tm_mon + 1,
             local_time->tm_year + 1900,
             local_time->tm_hour,
             local_time->tm_min);
    return formatted_date;
}

// Function to get the file size and store it as a string
void get_file_size(const char *file_name, char *file_size_str, char size_str_len) {
    char full_file_name[100];
    snprintf(full_file_name, sizeof(full_file_name), "%s.txt", file_name);
    FILE *file = fopen(full_file_name, "rb"); // Open the file in binary read mode
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        snprintf(file_size_str, size_str_len, "Error");
        return;
    }
    fseek(file, 0, SEEK_END); // Move the file pointer to the end
    long size = ftell(file);  // Get the file size
    fclose(file);             // Close the file
    snprintf(file_size_str, size_str_len, "%ld", size); // Convert size to string
}

void add_file_record(const char *file_name) {
    Properties data;
    char file_size_str[50];
    get_file_size(file_name, file_size_str, sizeof(file_size_str));
    char *formatted_date = get_formatted_date();
    strncpy(data.name, file_name, sizeof(data.name) - 1);
    data.name[sizeof(data.name) - 1] = '\0';
    strncpy(data.date, formatted_date, sizeof(data.date) - 1);
    data.date[sizeof(data.date) - 1] = '\0';
    strncpy(data.size, file_size_str, sizeof(data.size) - 1);
    data.size[sizeof(data.size) - 1] = '\0';
    free(formatted_date);
    FILE *fp = fopen("file_record.txt", "a");
    if (fp == NULL) {
        fprintf(stderr, "file_record.txt is inaccessible!\n");
        return;
    }
    fprintf(fp, "%s %s %s_bytes\n", data.name, data.date, data.size);
    fclose(fp);
    printf("Record added: %s %s %s_bytes\n", data.name, data.date, data.size);
}

void delete_file_record(const char *file_name) {
    FILE *fp1, *fp2;
    char ch[256];
    Properties data;
    int record_found = 0;
    fp1 = fopen("file_record.txt", "r");
    if (fp1 == NULL) {
        fprintf(stderr, "record_file est inaccessible!!!\n");
        return;
    }
    fp2 = fopen("temp.txt", "w");
    if (fp2 == NULL) {
        fprintf(stderr, "Erreur lors de la création d'un fichier temporel!!!\n");
        fclose(fp1);
        return;
    }
    while (fgets(ch, 256, fp1) != NULL) {
        // Parse the line into the Properties structure
        if (sscanf(ch, "%49[^ ] %19[^ ] %49[^\n]", data.name, data.date, data.size) != 3) {
            fprintf(stderr, "Erreur lors de l'analyse du fichier!!!\n");
            continue;
        }
        if (strncmp(data.name, file_name, sizeof(data.name)) == 0) {
            record_found = 1; // Mark that the record was found
            continue;
        }
        fputs(ch, fp2);
    }
    fclose(fp1);
    fclose(fp2);
    if (!record_found) {
        printf("Aucun enregistrement trouvé pour le fichier: %s\n", file_name);
    } else {
        if (remove("file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors de la suppression du fichier original!!!\n");
            return;
        }
        if (rename("temp.txt", "file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors du renommage du fichier temporaire!!!\n");
            return;
        }
        printf("Enregistrement supprimé avec succès pour le fichier: %s\n", file_name);
    }
}
void update_file_record(const char *old_file_name, const char *new_file_name) {
    FILE *fp1, *fp2;
    char buffer[256];
    Properties data;
    int record_found = 0;
    fp1 = fopen("file_record.txt", "r");
    if (fp1 == NULL) {
        fprintf(stderr, "record_file est inaccessible!!!\n");
        return;
    }
    fp2 = fopen("temp.txt", "w");
    if (fp2 == NULL) {
        fprintf(stderr, "Erreur lors de la création d'un fichier temporel!!!\n");
        fclose(fp1);
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp1) != NULL) {
        if (sscanf(buffer, "%49[^ ] %19[^ ] %49[^\n]", data.name, data.date, data.size) != 3) {
            fprintf(stderr, "Erreur lors de l'analyse du fichier!!!\n");
            continue;
        }
        if (strncmp(data.name, old_file_name, sizeof(data.name)) == 0) {
            strncpy(data.name, new_file_name, sizeof(data.name));
            record_found = 1;
        }
        fprintf(fp2, "%s %s %s_bytes\n", data.name, data.date, data.size);
    }
    fclose(fp1);
    fclose(fp2);
    if (!record_found) {
        printf("Aucun enregistrement trouvé pour le fichier: %s\n", old_file_name);
        remove("temp.txt");
    } else {
        if (remove("file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors de la suppression du fichier original!!!\n");
            return;
        }
        if (rename("temp.txt", "file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors du renommage du fichier temporaire!!!\n");
            return;
        }
        printf("Enregistrement mis à jour avec succès pour le fichier: %s\n", old_file_name);
    }
}

// Function to update the size field of a record
void update_size_record(const char *file_name) {
    FILE *fp1, *fp2;
    char buffer[256];
    Properties data;
    int record_found = 0;
    char file_size_str[50];
    // Get the updated size of the file
    get_file_size(file_name, file_size_str, sizeof(file_size_str));
    // Open the file containing the records
    fp1 = fopen("file_record.txt", "r");
    if (fp1 == NULL) {
        fprintf(stderr, "record_file est inaccessible!!!\n");
        return;
    }
    // Create a temporary file
    fp2 = fopen("temp.txt", "w");
    if (fp2 == NULL) {
        fprintf(stderr, "Erreur lors de la création d'un fichier temporel!!!\n");
        fclose(fp1);
        return;
    }
    // Read each record and update the size if the file name matches
    while (fgets(buffer, sizeof(buffer), fp1) != NULL) {
        if (sscanf(buffer, "%49[^ ] %19[^ ] %49[^\n]", data.name, data.date, data.size) != 3) {
            fprintf(stderr, "Erreur lors de l'analyse du fichier!!!\n");
            continue;
        }
        if (strncmp(data.name, file_name, sizeof(data.name)) == 0) {
            // Update the size field
            strncpy(data.size, file_size_str, sizeof(data.size));
            record_found = 1; // Mark that the record was found
        }
        // Write the updated (or unchanged) record back to the temp file
        fprintf(fp2, "%s %s %s_bytes\n", data.name, data.date, data.size);
    }
    fclose(fp1);
    fclose(fp2);
    if (!record_found) {
        printf("Aucun enregistrement trouvé pour le fichier: %s\n", file_name);
        remove("temp.txt"); // Clean up the temporary file
    } else {
        // Replace the original file with the updated temp file
        if (remove("file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors de la suppression du fichier original!!!\n");
            return;
        }
        if (rename("temp.txt", "file_record.txt") != 0) {
            fprintf(stderr, "Erreur lors du renommage du fichier temporaire!!!\n");
            return;
        }
        printf("La taille a été mise à jour avec succès pour le fichier: %s\n", file_name);
    }
}

// Callback function for saving new file
static void save_new_file(GtkWidget *widget, gpointer data) {
    GtkWidget **widgets = (GtkWidget **) data;
    GtkWidget *title_entry = widgets[0];  // Title entry field
    GtkWidget *content_entry = widgets[1]; // Content entry field
    // Get the text from the entry fields (GtkEditable interface)
    const gchar *file_name = gtk_editable_get_text(GTK_EDITABLE(title_entry));
    // Get text from GtkTextView
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(content_entry));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    // Create the file name by appending ".txt"
    gchar *file_path = g_strdup_printf("%s.txt", file_name);
    // Open the file for writing
    FILE *fp = fopen(file_path, "w");
    if (fp == NULL) {
        g_print("Error opening file for writing\n");
        g_free(file_path);
        return;
    }
    // Write the text to the file
    fputs(text, fp);
    fclose(fp);
    add_file_record(file_name);
    g_print("File saved successfully: %s\n", file_path);
    // Add the file record to the UI (assuming you have the date and size)
    gchar *date = get_formatted_date();  // Replace with actual date logic
    gchar size_str[20];
    gsize file_size = strlen(text);
    g_snprintf(size_str, sizeof(size_str), "%zu", file_size);

    // Print size (or use it to display in the UI)
    g_print("File size: %s_bytes\n", size_str);
    g_free(file_path);
}

static GtkTextView *contenent = NULL;

static void add_new_file(GtkWidget *app, gpointer user_data) {
    GtkWidget *window, *grid, *title, *button_save, *button_discard, *title_label;
    GtkWidget **widgets = g_new(GtkWidget *, 2);  // To store entry widgets for later use

    // Create the main window
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Adding New File");

    // Create a grid to hold the widgets
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Create a label for the file name entry
    title_label = gtk_label_new("ENTRER LE NOM DU FICHIER ");
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 3, 1);  // Add label to grid

    // Create a GtkEntry widget (for file title)
    title = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), title, 5, 0, 5, 1);  // Add to grid

    // Text view with scrolled window
    contenent = GTK_TEXT_VIEW(gtk_text_view_new());
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(contenent));
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 2, 10, 10);
    gtk_widget_set_size_request(scrolled_window, 100, 500);

    // Store the entry widgets in the array
    widgets[0] = title;
    widgets[1] = GTK_WIDGET(contenent);

    // Button 1 - Save
    button_save = gtk_button_new_with_label("Save");
    gtk_grid_attach(GTK_GRID(grid), button_save, 2, 512, 2, 2);
    g_signal_connect(button_save, "clicked", G_CALLBACK(save_new_file), widgets); // Connect the save action

    // Button 2 - Discard (just closes the window)
    button_discard = gtk_button_new_with_label("Discard");
    gtk_grid_attach(GTK_GRID(grid), button_discard, 6, 512, 2, 2);
    g_signal_connect(button_discard, "clicked", G_CALLBACK(gtk_window_close), window);  // Close the window

    // Show all the widgets
    gtk_window_present(GTK_WINDOW(window));
}

// Callback function for file deletion
static void supprimer(GtkWidget *widget, gpointer data) {
    GtkWidget **widgets = (GtkWidget **) data;
    GtkWidget *title_entry = widgets[0];  // Title entry field

    // Get the text from the entry field (GtkEditable interface)
    const gchar *file_name = gtk_editable_get_text(GTK_EDITABLE(title_entry));

    // Create the file name by appending ".txt"
    gchar *file_path = g_strdup_printf("%s.txt", file_name);

    // Attempt to delete the file
    if (remove(file_path) == 0) {
        delete_file_record(file_name);
        g_print("File deleted successfully: %s\n", file_path);
    } else {
        g_print("Error deleting file: %s\n", file_path);
    }
    g_free(file_path);
}

// Callback function to manage file actions
static void delete_file(GtkWidget *app, gpointer user_data) {
    GtkWidget *window, *button_save, *button_discard;
    GtkWidget *grid, *title, *title_label;
    GtkWidget **widgets = g_new(GtkWidget *, 1);  // To store entry widget for later use

    // Create a new window
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Deleting File");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 200);

    // Create the grid container for buttons
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Create a label for the file name entry
    title_label = gtk_label_new("ENTRER LE NOM DU FICHIER: ");
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 10, 2);  // Add label to grid

    // Create a GtkEntry widget (for file title)
    title = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), title, 10, 0, 10, 2);  // Add to grid

    // Store the entry widgets in the array
    widgets[0] = title;

    // Button 1 - delete
    button_save = gtk_button_new_with_label("delete");
    gtk_grid_attach(GTK_GRID(grid), button_save, 5, 2, 2, 2);
    g_signal_connect(button_save, "clicked", G_CALLBACK(supprimer), widgets); // Connect delete action

    // Button 2 - Discard (just closes the window)
    button_discard = gtk_button_new_with_label("Discard");
    gtk_grid_attach(GTK_GRID(grid), button_discard, 10, 2, 2, 2);
    g_signal_connect(button_discard, "clicked", G_CALLBACK(gtk_window_close), window);  // Close the window

    // Show the window and all its widgets
    gtk_window_present(GTK_WINDOW(window));
}

static GtkTextView *textview = NULL;

void save(GtkWidget *widget, gpointer data) {
    GtkWidget **filename = (GtkWidget **)data;

    // Retrieve the GtkEntry widget and its text
    const gchar *file_name = gtk_editable_get_text(GTK_EDITABLE(filename[0]));

    // Validate the file name
    if (file_name == NULL || strlen(file_name) == 0) {
        g_warning("No file name provided.");
        return;  // Exit if no file name is given
    }
    gchar *file_path = g_strdup_printf("%s.txt", file_name);

    // Retrieve the content from the GtkTextView buffer
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);

    gchar *content = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Write the content to the file
    if (!g_file_set_contents(file_path, content, -1, NULL)) {
        g_warning("Error: Failed to save file: `%s`", file_path);
    } else {
        update_size_record(file_name);
        g_message("File saved successfully: `%s`", file_path);
    }

    // Free allocated memory
    g_free(file_path);
    g_free(content);
}


void open(GtkWidget *widget, gpointer data) {
    GtkWidget **filename = (GtkWidget **)data;

    // Retrieve the GtkEntry widget and its text
    const gchar *file_name = gtk_editable_get_text(GTK_EDITABLE(filename[0]));

    // Validate the file name
    if (file_name == NULL || strlen(file_name) == 0) {
        g_warning("No file name provided.");
        return;  // Exit if no file name is given
    }

    gchar *file_path = g_strdup_printf("%s.txt", file_name);

    // Attempt to open the file
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        g_warning("Error: Unable to open the file: `%s`", file_path);
        return;  // Exit if file couldn't be opened
    }

    // Read the file content and set it to the GtkTextView buffer
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gchar *content = NULL;
    gsize length;
    if (g_file_get_contents(file_path, &content, &length, NULL)) {
        gtk_text_buffer_set_text(buffer, content, length);
        g_free(content);
    } else {
        g_warning("Error: Failed to read file content.");
    }
    fclose(file);
}

static void edit_file(GtkApplication *app, gpointer user_data) {
    GtkWidget **filename = g_new(GtkWidget *, 1);  // To store entry widget for later use
    GtkWidget *grid, *title_label;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "File Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create the grid container for buttons
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    title_label = gtk_label_new("         ENTRER LE NOM DU FICHIER ");
    gtk_grid_attach(GTK_GRID(grid), title_label, 10, 0, 40, 2);  // Add label to grid

    // Create a GtkEntry widget (for file title)
    GtkWidget *file_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), file_name, 50, 0, 10, 2);  // Add to grid
    // Store the entry widgets in the array
    filename[0] = file_name;

    // Text view with scrolled window
    textview = GTK_TEXT_VIEW(gtk_text_view_new());
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(textview));

    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 5, 2, 100, 100);
    gtk_widget_set_size_request(scrolled_window, 100, 500);

    // Create buttons
    GtkWidget *open_button = gtk_button_new_with_label("Open");
    GtkWidget *save_button = gtk_button_new_with_label("Save");
    GtkWidget *discard_button = gtk_button_new_with_label("Discard");

    // Attach buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), open_button, 70, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), save_button, 20, 520, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), discard_button, 30, 520, 2, 1);

    // Connect signals to button callbacks
    g_signal_connect(open_button, "clicked", G_CALLBACK(open), filename);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save), filename);
    g_signal_connect(discard_button, "clicked", G_CALLBACK(gtk_window_close), window);

    // Show the window and all its widgets
    gtk_window_present(GTK_WINDOW(window));
}


static void renommer(GtkWidget *widget, gpointer data){
    GtkWidget **widgets = (GtkWidget **) data;
    GtkWidget *old_name = widgets[0];  // old name entry field
    GtkWidget *new_name = widgets[1];  // new name entry field
    FILE *fp1, *fp2;
    char ch[256];
    int i = 0;

    // Get the texts from the entry fields (GtkEditable interface)
    const gchar *old_file_name = gtk_editable_get_text(GTK_EDITABLE(old_name));
    const gchar *new_file_name = gtk_editable_get_text(GTK_EDITABLE(new_name));

    // Create the files name by appending ".txt"
    gchar *old_file_path = g_strdup_printf("%s.txt", old_file_name);
    gchar *new_file_path = g_strdup_printf("%s.txt", new_file_name);

    fp1 = fopen(old_file_path, "r");
    if (fp1 == NULL) {
        printf("File '%s' is inaccessible!\n", old_file_name);
        g_free(old_file_path);
        g_free(new_file_path);
        return;
    }
    fp2 = fopen(new_file_path, "w");
    while (fgets(ch, 256, fp1) != NULL) {
        i++;
        fputs(ch, fp2);
    }
    fclose(fp1);
    fclose(fp2);

    // Now delete the old file and update the record
    if (remove(old_file_path) == 0) {
        gchar size_str[20];
        g_snprintf(size_str, sizeof(size_str), "%zu", strlen(new_file_name)); // Example size
        update_file_record(old_file_name, new_file_name);
        printf("File '%s' renamed to '%s' successfully.\n", old_file_name, new_file_name);
    } else {
        printf("Error deleting file '%s'\n", old_file_path);
    }

    g_free(old_file_path);
    g_free(new_file_path);
}


static void rename_file(GtkWidget *app, gpointer user_data){
    GtkWidget *window, *button_save, *button_discard;
    GtkWidget *grid, *old_name, *new_name, *title_label1, *title_label2;
    GtkWidget **widgets = g_new(GtkWidget *, 2);  // To store entry widget for later use

     // Create a new window for file management and set its title
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "rename file");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 200);

    // Create the grid container for buttons
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Create a label for the file name entry
    title_label1 = gtk_label_new("ENTRER L'ANCIEN NOM: ");
    gtk_grid_attach(GTK_GRID(grid), title_label1, 0, 0, 10, 2);  // Add label to grid


    // Create a GtkEntry widget (for file title)
    old_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), old_name, 10, 0, 10, 2 );  // Add to grid

    // Create a label for the file name entry
    title_label2 = gtk_label_new("ENTRER LE NOUVEAU NOM: ");
    gtk_grid_attach(GTK_GRID(grid), title_label2, 0, 2, 10, 2);  // Add label to grid


    // Create a GtkEntry widget (for file title)
    new_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), new_name, 10, 2, 10, 2);  // Add to grid

    // Store the entry widgets in the array
    widgets[0] = old_name;
    widgets[1] = new_name;

    // Button 1 - save change
    button_save = gtk_button_new_with_label("rename");
    gtk_grid_attach(GTK_GRID(grid), button_save, 5, 4, 2, 2);
    g_signal_connect(button_save, "clicked", G_CALLBACK(renommer), widgets); // Connect delete action

    // Button 2 - Discard (just closes the window)
    button_discard = gtk_button_new_with_label("Discard");
    gtk_grid_attach(GTK_GRID(grid), button_discard, 10, 4, 2, 2);
    g_signal_connect(button_discard, "clicked", G_CALLBACK(gtk_window_close), window);  // Close the window

    // Show the window and all its widgets
    gtk_window_present(GTK_WINDOW(window));

}

typedef struct noeud {
    char name[50];
    char date[50];
    char size[50];
    struct noeud *suiv;
} Noeud;

// Define the structure for the queue
typedef struct file {
    Noeud *tete;
    Noeud *queue;
} File;

// Function to enqueue a record
void enfiler(File *file_record, char file_name[50], char file_date[50], char file_size[50]) {
    Noeud *nouveau = (Noeud *)malloc(sizeof(Noeud));
    if (nouveau == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    strcpy(nouveau->name, file_name);
    strcpy(nouveau->date, file_date);
    strcpy(nouveau->size, file_size);
    nouveau->suiv = NULL;

    if (file_record->tete == NULL) {
        file_record->tete = nouveau;
        file_record->queue = nouveau;
    } else {
        file_record->queue->suiv = nouveau;
        file_record->queue = nouveau;
    }
}

// Function to dequeue a record
Noeud* defiler(File *file_record) {
    if (file_record->tete == NULL) {
        printf("The queue is empty!\n");
        return NULL;
    }

    Noeud *temp = file_record->tete;
    if (file_record->tete == file_record->queue) {
        file_record->tete = NULL;
        file_record->queue = NULL;
    } else {
        file_record->tete = file_record->tete->suiv;
    }

    temp->suiv = NULL;
    return temp;
}

// Function to load records from a file
void get_file_record(File *file_record) {
    FILE *fp = fopen("file_record.txt", "r");
    char buffer[256], name[50], date[50], size[50];

    if (fp == NULL) {
        printf("Cannot access file_record.txt\n");
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (sscanf(buffer, "%49s %49s %49s", name, date, size) == 3) {
            enfiler(file_record, name, date, size);
        }
    }
    fclose(fp);
}

// Function to clear children from a GtkBox
void clear_box_children(GtkWidget *box) {
    GtkWidget *child = gtk_widget_get_first_child(box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }
}

// Function to display records in the GtkBox
void show_record(GtkWidget *app, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data); // Cast the user_data back to GtkBox
    GtkWidget *label1, *label2, *label3, *grid, *title_label1, *title_label2, *title_label3, *espace1, *espace2, *esp1, *esp2;
    clear_box_children(box);
    int i = 1;

    File *file_record = (File *)malloc(sizeof(File));
    file_record->tete = NULL;
    file_record->queue = NULL;
    get_file_record(file_record);

    // Create a new grid to display records
    grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(box), grid);  // Add grid to the box

    // Create a label for name
    title_label1 = gtk_label_new("NAME");
    gtk_grid_attach(GTK_GRID(grid), title_label1, 0, 0, 5, 1);  // Add label to grid

    espace1 = gtk_label_new("    ");
    gtk_grid_attach(GTK_GRID(grid), espace1, 5, 0, 5, 1);  // Add label to grid

    // Create a label for date
    title_label2 = gtk_label_new("DATE");
    gtk_grid_attach(GTK_GRID(grid), title_label2, 10, 0, 5, 1);  // Add label to grid

    espace2 = gtk_label_new("    ");
    gtk_grid_attach(GTK_GRID(grid), espace2, 15, 0, 5, 1);  // Add label to grid

    // Create a label for size
    title_label3 = gtk_label_new("SIZE");
    gtk_grid_attach(GTK_GRID(grid), title_label3, 20, 0, 5, 1);  // Add label to grid

    // Display records in the grid
    Noeud *current;
    while ((current = defiler(file_record)) != NULL) {
        // Create labels for each record
        label1 = gtk_label_new(current->name);
        gtk_grid_attach(GTK_GRID(grid), label1, 0, i, 5, 1);

        esp1 = gtk_label_new("    ");
        gtk_grid_attach(GTK_GRID(grid), esp1, 5, i, 5, 1);

        label2 = gtk_label_new(current->date);
        gtk_grid_attach(GTK_GRID(grid), label2, 10, i, 5, 1);

        esp2 = gtk_label_new("    ");
        gtk_grid_attach(GTK_GRID(grid), esp2, 15, i, 5, 1);

        label3 = gtk_label_new(current->size);
        gtk_grid_attach(GTK_GRID(grid), label3, 20, i, 5, 1);
        printf(current->size);

        free(current);
        i++;
    }
    free(file_record);
}

static void gestionnaire_des_fichiers(GtkWidget *widget, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid, *file_record_box;
    GtkWidget *button1, *button2, *button3, *button4, *button5;

    // Create a new window for file management and set its title
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "File Management");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 200);

    // Create the grid container
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Create a GtkBox for displaying records
    file_record_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 100);
    gtk_grid_attach(GTK_GRID(grid), file_record_box, 0, 4, 20, 10); // Add the box to the grid

    // Button 1 - Add New File
    button1 = gtk_button_new_with_label("ADD NEW FILE");
    gtk_grid_attach(GTK_GRID(grid), button1, 0, 0, 5, 2);
    // Pass the grid as user_data to the callback function
    g_signal_connect(button1, "clicked", G_CALLBACK(add_new_file), NULL);

    // Button 2 - Edit File
    button2 = gtk_button_new_with_label("OPEN FILE");
    gtk_grid_attach(GTK_GRID(grid), button2, 5, 0, 5, 2);
    g_signal_connect(button2, "clicked", G_CALLBACK(edit_file), NULL);

    // Button 3 - rename File
    button3 = gtk_button_new_with_label("RENAME FILE");
    gtk_grid_attach(GTK_GRID(grid), button3, 10, 0, 5, 2);
    g_signal_connect(button3, "clicked", G_CALLBACK(rename_file), NULL);

    // Button 4 - Delete File
    button4 = gtk_button_new_with_label("DELETE FILE");
    gtk_grid_attach(GTK_GRID(grid), button4, 15, 0, 5, 2);
    g_signal_connect(button4, "clicked", G_CALLBACK(delete_file), NULL);

    // Button 5 - actualiser
    button5 = gtk_button_new_with_label("refresh");
    gtk_grid_attach(GTK_GRID(grid), button5, 20, 0, 5, 2);
    g_signal_connect(button5, "clicked", G_CALLBACK(show_record), file_record_box);

    // Show the window and all its widgets
    gtk_window_present(GTK_WINDOW(window));
}


// Global variables
GtkEntry *entry_expression;
GtkEntry *entry_result;
GtkEntryBuffer *buffer_expression;
GtkEntryBuffer *buffer_result;

// Definition of the stack
typedef struct cellule {
    char val[1];  // Single character stored here
    struct cellule *suiv;  // Pointer to the next element
} Cellule;

typedef struct pile {
    Cellule *sommet;  // Pointer to the top of the stack
} Pile;

// Function to get the top value of the stack
char sommet(Pile *p) {
    if (p->sommet == NULL) {
        printf("Pile est vide!!!\n");
        return '\0';  // Return null character if the stack is empty
    } else {
        return p->sommet->val[0];  // Return the top value
    }
}

// Function to push an element onto the stack
void empiler(Pile *p, char e) {
    Cellule *nouveau = (Cellule*)malloc(sizeof(Cellule));  // Allocate memory for a new cell
    if (!nouveau) {
        printf("Erreur d'allocation de mémoire.\n");
        return;
    }
    nouveau->val[0] = e;  // Assign the value to the new cell
    nouveau->suiv = p->sommet;  // Point the new cell to the previous top of the stack
    p->sommet = nouveau;  // Update the stack's top to the new cell
}

// Function to pop an element from the stack
void depiler(Pile *p) {
    if (p->sommet == NULL) {
        printf("La pile est déjà vide!!!\n");
        return;
    } else {
        Cellule *temp = p->sommet;  // Store the current top
        p->sommet = p->sommet->suiv;  // Move the top to the next element
        free(temp);  // Free the memory of the old top
    }
}

// Priority function for operators
int priorite(char op) {
    if (op == '+' || op == '-') {
        return 1;
    }
    if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

// Function to evaluate the postfix expression (assuming all operands are single digits)
int evaluerPostfix(char *expr) {
    Pile pile;
    pile.sommet = NULL;

    for (int i = 0; expr[i] != '\0'; i++) {
        if (isdigit(expr[i])) {
            empiler(&pile, expr[i]);  // Push the character (digit)
        } else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            // Pop two operands and apply the operator
            float b = sommet(&pile) - '0';  // Convert from char to int
            depiler(&pile);
            float a = sommet(&pile) - '0';  // Convert from char to int
            depiler(&pile);

            double result;
            switch (expr[i]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = (double)a / b; break;
            }
            empiler(&pile, result + '0');  // Push result back as char
        }
    }
    return sommet(&pile) - '0';  // Return the final result as an integer
}

// Function to convert infix to postfix using Shunting Yard
void infixToPostfix(char *infix, char *postfix) {
    Pile operateur;
    operateur.sommet = NULL;
    int j = 0;

    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];

        if (isdigit(c)) {
            postfix[j++] = c;  // Add digit to the output
        } else if (c == '(') {
            empiler(&operateur, c);  // Push '(' to the stack
        } else if (c == ')') {
            // Pop operators until '(' is encountered
            while (sommet(&operateur) != '(') {
                postfix[j++] = sommet(&operateur);
                depiler(&operateur);
            }
            depiler(&operateur);  // Pop '(' from the stack
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            // Pop operators with higher or equal precedence
            while (operateur.sommet != NULL && priorite(sommet(&operateur)) >= priorite(c)) {
                postfix[j++] = sommet(&operateur);
                depiler(&operateur);
            }
            empiler(&operateur, c);  // Push the current operator to the stack
        }
    }

    // Pop remaining operators from the stack
    while (operateur.sommet != NULL) {
        postfix[j++] = sommet(&operateur);
        depiler(&operateur);
    }

    postfix[j] = '\0';  // Null-terminate the postfix expression
}

// Event handler for the "=" button
void on_button_calculate(GtkButton *button, gpointer user_data) {
    const char *text = gtk_entry_buffer_get_text(buffer_expression);
    char postfix[100];
    infixToPostfix((char*)text, postfix);

    printf("Postfix expression: %s\n", postfix);

    // Evaluate the postfix expression
    int result = evaluerPostfix(postfix);
    char result_str[20];
    sprintf(result_str, "%d", result);

    gtk_entry_buffer_set_text(buffer_result, result_str, -1);  // Show the result
}

// Event handler for numeric and operator buttons
void on_button_click(GtkButton *button, gpointer user_data) {
    const char *button_label = gtk_button_get_label(button);
    gtk_entry_buffer_insert_text(buffer_expression,
    gtk_entry_buffer_get_length(buffer_expression), button_label, -1);
}

// Event handler for the "AC" button (clear)
void on_button_clear(GtkButton *button, gpointer user_data) {
    gtk_entry_buffer_set_text(buffer_expression, "", -1);
    gtk_entry_buffer_set_text(buffer_result, "", -1);
}

// Event handler for the backspace button
void on_button_backspace(GtkButton *button, gpointer user_data) {
    gsize length = gtk_entry_buffer_get_length(buffer_expression);
    if (length > 0) {
        gtk_entry_buffer_delete_text(buffer_expression, length - 1, 1);
    }
}

// Fonction pour créer un bouton avec un gestionnaire
GtkWidget *create_button(const char *label, GCallback callback) {
    GtkWidget *button = gtk_button_new_with_label(label);
    g_signal_connect(button, "clicked", callback, NULL);
    return button;
}

// Fonction principale de l'application
static void calculatrice(GtkApplication *app, gpointer user_data) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GtkCssProvider *css_provider_button = gtk_css_provider_new();

    // Charger le CSS pour la fenêtre et les boutons
    gtk_css_provider_load_from_string(css_provider,
        "window {"
        "  background-image: url('file:///C:/Users/DELL PRO/Desktop/PROJECT1/PROJECT1/calc_back.jpg');"
        "  background-size: cover;"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}"
        "button {"
        "  font-size: 20px;"
        "  margin: 2px;"
        "  border-radius: 5px;"
        "  color: #ffffff;"
        "  background-color: #333333;"
        "}"
        "button:hover {"
        "  background-color: #555555;"
        "}");

    // Création de la fenêtre principale
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Calculatrice");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);

    // Appliquer le style à la fenêtre
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(window));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Création d'une grille pour organiser les widgets
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Création des buffers pour les champs d'entrée
    buffer_expression = gtk_entry_buffer_new(NULL, -1);
    buffer_result = gtk_entry_buffer_new(NULL, -1);

    // Champs d'entrée pour l'expression et le résultat
    entry_expression = GTK_ENTRY(gtk_entry_new_with_buffer(buffer_expression));
    entry_result = GTK_ENTRY(gtk_entry_new_with_buffer(buffer_result));

    gtk_editable_set_editable(GTK_EDITABLE(entry_result), FALSE); // Résultat non modifiable
    gtk_entry_set_placeholder_text(entry_expression, "Saisissez une expression...");
    gtk_entry_set_placeholder_text(entry_result, "Résultat...");

    // Ajouter les champs d'entrée à la grille
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(entry_expression), 0, 0, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(entry_result), 0, 1, 4, 1);

    // Boutons de la calculatrice
    const char *buttons[] = {
        "7", "8", "9", "+",
        "4", "5", "6", "-",
        "1", "2", "3", "*",
        "(", "0", ")", "/",
        ".", "AC", "←", "=",
        NULL
    };

    // Ajouter les boutons à la grille
    int row = 2, col = 0;
    for (int i = 0; buttons[i] != NULL; i++) {
        GtkWidget *button;

        if (strcmp(buttons[i], "AC") == 0) {
            button = create_button(buttons[i], G_CALLBACK(on_button_clear));
            GtkStyleContext *context_button = gtk_widget_get_style_context(GTK_WIDGET(button));
            gtk_style_context_add_provider(context_button, GTK_STYLE_PROVIDER(css_provider_button), GTK_STYLE_PROVIDER_PRIORITY_USER);
        } else if (strcmp(buttons[i], "←") == 0) {
            button = create_button(buttons[i], G_CALLBACK(on_button_backspace));
            GtkStyleContext *context_button = gtk_widget_get_style_context(GTK_WIDGET(button));
            gtk_style_context_add_provider(context_button, GTK_STYLE_PROVIDER(css_provider_button), GTK_STYLE_PROVIDER_PRIORITY_USER);
        } else if (strcmp(buttons[i], "=") == 0) {
            button = create_button(buttons[i], G_CALLBACK(on_button_calculate));
            GtkStyleContext *context_button = gtk_widget_get_style_context(GTK_WIDGET(button));
            gtk_style_context_add_provider(context_button, GTK_STYLE_PROVIDER(css_provider_button), GTK_STYLE_PROVIDER_PRIORITY_USER);
        } else {
            button = create_button(buttons[i], G_CALLBACK(on_button_click));
            GtkStyleContext *context_button = gtk_widget_get_style_context(GTK_WIDGET(button));
            gtk_style_context_add_provider(context_button, GTK_STYLE_PROVIDER(css_provider_button), GTK_STYLE_PROVIDER_PRIORITY_USER);
        }

        gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        col++;
        if (col == 4) {
            col = 0;
            row++;
        }
    }

    // Ajouter la grille à la fenêtre
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Afficher la fenêtre
    gtk_window_present(GTK_WINDOW(window));
}

static void important(GtkWidget *button, gpointer user_data){
    GtkWidget *content_box = GTK_WIDGET(user_data);
    // Supprimer le contenu de content_box
    while (content_box != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(content_box);
        gtk_widget_unparent(content_box);
        content_box = next;
    }

}

void show_task_record();
static void delete_task();
static void ma_journee();

// Structure pour gérer les données des tâches
typedef struct {
    GtkLabel *task_label;
    GtkWidget *check_button, *delete_button;
    gboolean is_striked; // État : barré ou non
} TaskData;

// Fonction pour obtenir l'heure actuelle sous forme de chaîne
char *get_date() {
    time_t raw_time;
    struct tm *time_info;
    char *buffer = malloc(64); // Allouer de la mémoire pour le format de date

    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, 64, "%Y-%m-%d %H:%M", time_info);
    return buffer;
}

// Fonction pour mettre à jour le label de l'heure
gboolean update_time_label(GtkLabel *label) {
    char *formatted_date = get_date(); // Obtenir l'heure actuelle
    gtk_label_set_text(label, formatted_date);
    free(formatted_date); // Libérer la mémoire
    return TRUE; // Relancer la minuterie
}

// Fonction pour basculer entre barrer et dé-barrer le texte d'une tâche
static void toggle_strike_task(GtkButton *button, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    // Créer une liste d'attributs Pango
    PangoAttrList *attr_list = pango_attr_list_new();

    if (data->is_striked) {
        // Dé-barrer le texte
        data->is_striked = FALSE;
    } else {
        // Barrer le texte
        PangoAttribute *strikethrough = pango_attr_strikethrough_new(TRUE);
        pango_attr_list_insert(attr_list, strikethrough);
        data->is_striked = TRUE;
    }

    // Mettre à jour les attributs du texte
    gtk_label_set_attributes(data->task_label, attr_list);
    pango_attr_list_unref(attr_list); // Libérer la liste des attributs
}


static void delete_task(GtkButton *button, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;
    const char *task_to_delete = gtk_label_get_text(data->task_label);
    FILE *file, *temp_file;
    char line[256];

    // Ouvrir le fichier en lecture et un fichier temporaire en écriture
    file = fopen("task_record.txt", "r");
    temp_file = fopen("task_record_temp.txt", "w");

    if (file == NULL || temp_file == NULL) {
        printf("Erreur lors de l'ouverture des fichiers\n");
        if (file) fclose(file);
        if (temp_file) fclose(temp_file);
        return;
    }

    // Copier les lignes qui ne correspondent pas à la tâche à supprimer
    while (fgets(line, sizeof(line), file)) {
        // Supprimer le saut de ligne pour la comparaison
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, task_to_delete) != 0) {
            fprintf(temp_file, "%s\n", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Remplacer le fichier d'origine par le fichier temporaire
    remove("task_record.txt");
    rename("task_record_temp.txt", "task_record.txt");

    // Supprimer uniquement les widgets associés à cette tâche
    GtkWidget *grid = gtk_widget_get_parent(GTK_WIDGET(data->task_label));

    // Directly unparent the task label, check button, and delete button from the grid
    gtk_widget_unparent(GTK_WIDGET(data->task_label));   // Remove the task label
    gtk_widget_unparent(GTK_WIDGET(data->check_button)); // Remove the check button
    gtk_widget_unparent(GTK_WIDGET(data->delete_button)); // Remove the delete button

    // Free the dynamically allocated memory for TaskData
    g_free(data); // Free the memory allocated for TaskData
}

// Fonction pour afficher toutes les tâches depuis le fichier
void show_task_record(GtkWidget *content_grid) {
    FILE *fp;
    char line[256];
    int task_row = 2; // Compteur pour positionner les tâches
    GtkWidget *delete_button;


    // Ouvrir le fichier en mode lecture
    fp = fopen("task_record.txt", "r");
    if (fp == NULL) {
        printf("Erreur lors de l'ouverture du fichier !\n");
        return;
    }

    // Lire chaque ligne du fichier et ajouter chaque tâche dans un label
    while (fgets(line, sizeof(line), fp)) {
        // Supprimer le saut de ligne à la fin de la ligne lue
        line[strcspn(line, "\n")] = 0;

        // Créer un nouveau label pour chaque ligne (tâche)
        GtkWidget *task_label = gtk_label_new(line);

        // Ajouter le label à la grille
        gtk_grid_attach(GTK_GRID(content_grid), task_label, 0, task_row, 3, 1);

        // Ajouter un bouton "✔" pour barrer/dé-barrer
        GtkWidget *check_button = gtk_button_new_with_label("✔");
        gtk_grid_attach(GTK_GRID(content_grid), check_button, 3, task_row, 1, 1);

        // Ajouter un bouton "X" pour supprimer la tâche
        delete_button = gtk_button_new_with_label("X");
        gtk_grid_attach(GTK_GRID(content_grid), delete_button, 4, task_row, 1, 1);

        // Préparer les données pour le bouton
        TaskData *task_data = g_new(TaskData, 1);
        task_data->task_label = GTK_LABEL(task_label);
        task_data->check_button = GTK_WIDGET(check_button);
        task_data->delete_button = GTK_WIDGET(delete_button);
        task_data->is_striked = FALSE;

        // Connecter le bouton pour basculer le style de la tâche
        g_signal_connect(check_button, "clicked", G_CALLBACK(toggle_strike_task), task_data);

        // Connecter le bouton de suppression
        g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_task), task_data);

        // Afficher les widgets
        gtk_widget_show(task_label);
        gtk_widget_show(check_button);
        gtk_widget_show(delete_button);


        // Passer à la ligne suivante pour la tâche suivante
        task_row++;
    }
    fclose(fp); // Fermer le fichier
}

int count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return -1; // Erreur d'ouverture
    }

    int line_count = 0;
    char ch;

    // Lire chaque caractère du fichier
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++; // Incrémenter le compteur de lignes pour chaque saut de ligne
        }
    }
    fclose(file); // Fermer le fichier
    return line_count;
}

static void add_task(GtkButton *button, gpointer user_data) {
    GtkWidget *content_grid = GTK_WIDGET(user_data);

    int task_row =2+ count_lines_in_file("task_record.txt")*2; // Compteur pour positionner les tâches
    FILE *fp;
    GtkWidget *delete_button;

    // Récupérer le buffer associé à la zone de saisie
    GtkWidget *entry_widget = gtk_grid_get_child_at(GTK_GRID(content_grid), 0, 1);
    GtkEntry *entry = GTK_ENTRY(entry_widget);
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    // Obtenir le texte depuis le buffer
    const char *text = gtk_entry_buffer_get_text(buffer);

    if (text && *text) { // Vérifier si le texte n'est pas vide
        // Créer le label pour la tâche
        GtkWidget *task_label = gtk_label_new(text);
        gtk_grid_attach(GTK_GRID(content_grid), task_label, 0, task_row, 3, 1);

        // Ajouter un bouton "✔" pour barrer/dé-barrer
        GtkWidget *check_button = gtk_button_new_with_label("✔");
        gtk_grid_attach(GTK_GRID(content_grid), check_button, 3, task_row, 1, 1);

        // Ajouter un bouton "X" pour supprimer la tâche
        delete_button = gtk_button_new_with_label("X");
        gtk_grid_attach(GTK_GRID(content_grid), delete_button, 4, task_row, 1, 1);

        // Préparer les données pour le bouton
        TaskData *task_data = g_new(TaskData, 1);
        task_data->task_label = GTK_LABEL(task_label);
        task_data->check_button = GTK_WIDGET(check_button);
        task_data->delete_button = GTK_WIDGET(delete_button);
        task_data->is_striked = FALSE;

        // Connecter le bouton pour basculer le style de la tâche
        g_signal_connect(check_button, "clicked", G_CALLBACK(toggle_strike_task), task_data);

        // Connecter le bouton de suppression
        g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_task), task_data);

        // Afficher les widgets
        gtk_widget_show(task_label);
        gtk_widget_show(check_button);
        gtk_widget_show(delete_button);

        // Ouvrir le fichier en mode ajout pour enregistrer la tâche
        fp = fopen("task_record.txt", "a");
        if (fp == NULL) {
            printf("Erreur lors de l'ouverture du fichier task_record.txt\n");
            return;
        }

        // Écrire la tâche dans le fichier
        fprintf(fp, "%s\n", text);

        // Fermer le fichier
        fclose(fp);

        // Réinitialiser la zone de saisie via le buffer
        gtk_entry_buffer_set_text(buffer, "", -1);

        // Avancer à la ligne suivante pour la prochaine tâche
        task_row++;
    }
}

// Fonction appelée par le bouton "MA JOURNEE"
static void ma_journee(GtkWidget *button, gpointer user_data) {
    GtkWidget *content_box = GTK_WIDGET(user_data);
    GtkWidget *content_grid, *time_label, *add_button;
    GtkEntry *entry_expression;

    // Supprimer tous les enfants de content_box
    while (gtk_widget_get_first_child(content_box) != NULL) {
        GtkWidget *child = gtk_widget_get_first_child(content_box);
        gtk_widget_unparent(child);
    }

    // Créer un nouveau GtkGrid pour organiser les widgets
    content_grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(content_box), content_grid);

    // Ajouter le label avec la date et l'heure
    time_label = gtk_label_new(NULL);

    gtk_widget_set_size_request(time_label, 400, 200); // Largeur minimale de 400px

    // Centrer le texte horizontalement et verticalement
    gtk_label_set_xalign(GTK_LABEL(time_label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(time_label), 0.5);
    gtk_grid_attach(GTK_GRID(content_grid), time_label, 0, 0, 4, 1);
    update_time_label(GTK_LABEL(time_label)); // Mise à jour immédiate
    g_timeout_add(60000, (GSourceFunc)update_time_label, time_label);

    // Créer un fournisseur CSS pour personnaliser les styles
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        css_provider,
        "label {"
        "  font-size: 50px;"
        "  font-weight: bold;"
        "  color: white;"
        "  background-color: rgba(0, 0, 0, 0.6);"
        "  padding: 10px;"
        "  border-radius: 10px;"
        "}"
        "window {"
        "  background-image: url('file:///C:/Users/DELL PRO/Desktop/PROJECT1/PROJECT1/day.jpg');"
        "  background-size: cover;"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}",
        -1);

    // Appliquer le style au label
    GtkStyleContext *label_context = gtk_widget_get_style_context(time_label);
    gtk_style_context_add_provider(label_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Appliquer le style au conteneur principal (arrière-plan)
    GtkStyleContext *content_context = gtk_widget_get_style_context(GTK_WIDGET(content_box));
    gtk_style_context_add_provider(content_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(css_provider); // Libérer le fournisseur CSS après utilisation

    // Zone de saisie pour la tâche
    entry_expression = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry_expression, "Saisissez une tâche...");
    gtk_grid_attach(GTK_GRID(content_grid), GTK_WIDGET(entry_expression), 0, 1, 3, 1);

    // Ajouter un bouton pour valider la tâche
    add_button = gtk_button_new_with_label("Ajouter");
    gtk_grid_attach(GTK_GRID(content_grid), add_button, 3, 1, 1, 1);

    // Connecter le bouton pour ajouter des tâches
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_task), content_grid);

    show_task_record(content_grid);

    // Afficher tous les widgets ajoutés
    gtk_widget_show(content_grid);
    gtk_widget_show(time_label);
    gtk_widget_show(GTK_WIDGET(entry_expression));
    gtk_widget_show(add_button);
}


static void to_do_list(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *grid, *menu_box, *content_box, *menu_grid, *button1, *button2;
    GtkCssProvider *css_provider1, *css_provider2;
    GtkStyleContext *context1,  *context2;

    // Créer une nouvelle fenêtre
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "TO_DO_LIST");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);

    // Créer la grille principale
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Créer des GtkBox pour les sections menu et contenu
    menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_grid_attach(GTK_GRID(grid), menu_box, 0, 0, 1, 1);

    // Créer un fournisseur CSS pour le fond d'écran de la fenêtre
    css_provider1 = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider1,
        "window {"
        "  background-image: url('file:///C:/Users/DELL PRO/Desktop/PROJECT1/PROJECT1/b.jpg');" /* Chemin de l'image */
        "  background-size: cover;"                  /* Adapte l'image à la taille de la fenêtre */
        "  background-repeat: no-repeat;"            /* Pas de répétition */
        "  background-position: center;"
        "}",
        -1 );

    // Créer un fournisseur CSS pour les boutons
    css_provider2 = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider2,
        "button {"
        "  min-width: 200px;"               /* Set the width of the button */
        "  min-height: 50px;"               /* Set the height of the button */
        "  font-size: 18px;"            /* Set the font size of the button text */
        "  padding: 10px 20px;"
        "  font-weight: bold;"
        "  color: black;"
        "  background-color: #000000;"
        "  border-radius: 10px;"
        "}"
        "button:hover {"
        "  background-color: #555555;"
        "}");

    // Appliquer le CSS pour le fond de la fenêtre
    GtkStyleContext *context_window = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context_window, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Créer une boîte de contenu
    content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_grid_attach(GTK_GRID(grid), content_box, 1, 0, 1, 1);

    // Créer une grille pour les boutons de menu
    menu_grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(menu_box), menu_grid);

    // Bouton 1 - "MA JOURNEE"
    button1 = gtk_button_new_with_label("MA JOURNEE");
    gtk_grid_attach(GTK_GRID(menu_grid), button1, 0, 0, 1, 1);
    context1 = gtk_widget_get_style_context(button1);
    gtk_style_context_add_provider(context1, GTK_STYLE_PROVIDER(css_provider2), GTK_STYLE_PROVIDER_PRIORITY_USER); // Apply css_provider2 here
    g_signal_connect(button1, "clicked", G_CALLBACK(ma_journee), content_box);

    // Bouton 2 - "IMPORTANT" (placeholder)
    button2 = gtk_button_new_with_label("IMPORTANT");
    gtk_grid_attach(GTK_GRID(menu_grid), button2, 0, 1, 1, 1);
    context2 = gtk_widget_get_style_context(button2);
    gtk_style_context_add_provider(context2, GTK_STYLE_PROVIDER(css_provider2), GTK_STYLE_PROVIDER_PRIORITY_USER); // Apply css_provider2 here
    g_signal_connect(button2, "clicked", G_CALLBACK(important), NULL);

    // Afficher la fenêtre
    gtk_window_present(GTK_WINDOW(window));
}







// Main activation function to create the main window
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid, *fixed;
    GtkWidget *button1, *button2, *button3, *button4, *button5, *button6;
    GtkWidget *image1, *image2, *image3, *image4, *image5, *image6, *logo;
    GtkStyleContext *context1,  *context2,  *context3,  *context4,  *context5,  *context6;
    GtkCssProvider *css_provider1, *css_provider2;

    // Create a CSS provider for custom styles
    css_provider1 = gtk_css_provider_new();
    css_provider2 = gtk_css_provider_new();

    gtk_css_provider_load_from_string(css_provider1,
        "button {"
        "  border-radius: 20%;"
        "  min-width: 150px;"
        "  min-height: 150px;"
        "}");

    // Create the main window and set its title
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "===== SIMULATEUR DE SYSTEME D'EXPLOITATION =====");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);


    gtk_css_provider_load_from_string(css_provider2,
        "window {"
        "  background-image: url('file:///C:/Users/DELL PRO/Desktop/PROJECT1/PROJECT1/background.jpg');"  /* Chemin de l'image */
        "  background-size: cover;"                  /* Adapte l'image à la taille de la fenêtre */
        "  background-repeat: no-repeat;"            /* Pas de répétition */
        "  background-position: center;"
        "}");

    // Appliquer le CSS à la fenêtre
    GtkStyleContext *context_window = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context_window, GTK_STYLE_PROVIDER(css_provider2), GTK_STYLE_PROVIDER_PRIORITY_USER);


    // Create a GtkFixed container
    fixed = gtk_fixed_new();
    gtk_window_set_child(GTK_WINDOW(window), fixed);

    // Create a grid container for buttons
    grid = gtk_grid_new();
    gtk_fixed_put(GTK_FIXED(fixed), grid, 50, 50);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    // Button 1 - files_txt (opens the file management window)
    button1 = gtk_button_new_with_label("files_txt");
    image1 = gtk_image_new_from_file("files.png");// Replace with your image file
    context1 = gtk_widget_get_style_context(button1);
    gtk_style_context_add_provider(context1, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button1), image1);
    g_signal_connect(button1, "clicked", G_CALLBACK(gestionnaire_des_fichiers), NULL);
    gtk_grid_attach(GTK_GRID(grid), button1, 0, 0, 1, 1);  // Position button1

    // Button 2 - calculator
    button2 = gtk_button_new_with_label("calculator");
    image2 = gtk_image_new_from_file("calculator.png");  // Replace with image file
    context2 = gtk_widget_get_style_context(button2);
    gtk_style_context_add_provider(context2, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button2), image2);
    g_signal_connect(button2, "clicked", G_CALLBACK(calculatrice), NULL);
    gtk_grid_attach(GTK_GRID(grid), button2, 1, 0, 1, 1);  // Position button2

    // Button 3 - todo_list
    button3 = gtk_button_new_with_label("todo_list");
    image3 = gtk_image_new_from_file("todolist.png");  // Replace with image file
    context3 = gtk_widget_get_style_context(button3);
    gtk_style_context_add_provider(context3, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button3), image3);
    g_signal_connect(button3, "clicked", G_CALLBACK(to_do_list), NULL);
    gtk_grid_attach(GTK_GRID(grid), button3, 0, 1, 1, 1);  // Position button3

    // Button 4 - com_rec
    button4 = gtk_button_new_with_label("com_rec");
    image4 = gtk_image_new_from_file("record.png");  // Replace with image file
    context4 = gtk_widget_get_style_context(button4);
    gtk_style_context_add_provider(context4, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button4), image4);
    g_signal_connect(button4, "clicked", G_CALLBACK(print_hello), NULL);
    gtk_grid_attach(GTK_GRID(grid), button4, 1, 1, 1, 1);  // Position button4

    // Button 5 - gamebar
    button5 = gtk_button_new_with_label("gamebar");
    image5 = gtk_image_new_from_file("gamebar.png");  // Replace with image file
    context5 = gtk_widget_get_style_context(button5);
    gtk_style_context_add_provider(context5, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button5), image5);
    g_signal_connect(button5, "clicked", G_CALLBACK(print_hello), NULL);
    gtk_grid_attach(GTK_GRID(grid), button5, 0, 2, 1, 1);  // Position button5

    // Button 6 - calendar
    button6 = gtk_button_new_with_label("calendar");
    image6 = gtk_image_new_from_file("calendar.png");  // Replace with image file
    context6 = gtk_widget_get_style_context(button6);
    gtk_style_context_add_provider(context6, GTK_STYLE_PROVIDER(css_provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_button_set_child(GTK_BUTTON(button6), image6);
    g_signal_connect(button6, "clicked", G_CALLBACK(print_hello), NULL);
    gtk_grid_attach(GTK_GRID(grid), button6, 1, 2, 1, 1);  // Position button6

    // Add the top-right image
    logo = gtk_image_new_from_file("ensi.png");  // Replace with your image file
    gtk_widget_set_size_request(logo, 100, 100);
    gtk_fixed_put(GTK_FIXED(fixed), logo, 650, 50);

    // Show the window and all its widgets
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Create the application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up
    g_object_unref(app);
    return status;
}
