
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ENTRIES 5
#define MAX_DATA_LENGTH 32
#define MAX_BUFFER_LENGTH 170
#define MAX_CANDIDATES 20

/* Interface selection */
typedef enum
{
    NONE = 0,
    RADIO,
    ETHERNET
} InterfaceType;

/* Buffers */
char Input_buffer[MAX_DATA_LENGTH] = "";

char Radio_buffer[MAX_BUFFER_LENGTH] = "";
char Ethernet_buffer[MAX_BUFFER_LENGTH] = "";

char multicast_buffer[MAX_BUFFER_LENGTH] = "";

/* Individual entries */
char Radio_entries[MAX_ENTRIES][MAX_DATA_LENGTH];
char Ethernet_entries[MAX_ENTRIES][MAX_DATA_LENGTH];

int Radio_count = 0;
int Ethernet_count = 0;

/* Current interface */
InterfaceType current_interface = NONE;

/* Multicast candidates */
char candidates[MAX_CANDIDATES][MAX_DATA_LENGTH];

bool candidate_checked[MAX_CANDIDATES];

int candidate_count = 0;


/* Function declarations */

void SelectInterface(InterfaceType interface);

int GetCurrentCount(void);

int GetRemainingCapacity(void);

bool AddEntry(const char *data);

bool AddManualData(void);

void SetManualInput(const char *data);

void UpdateMulticastBuffer(void);

bool IsDuplicate(const char *data);

void OpenMulticastSelection(void);

void MulticastCheckboxChanged(int index, bool checked);

void MulticastDone(void);

void DisplayBuffer(void);

void ReadLine(char *buffer, int size);

int ReadNumber(void);


/* Read string safely */

void ReadLine(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        /* Remove carriage return */
        buffer[strcspn(buffer, "\r")] = '\0';
    }
}


/* Read integer */

int ReadNumber(void)
{
    char input[32];
    char *end;
    long value;

    ReadLine(input, sizeof(input));

    value = strtol(input, &end, 10);

    while (*end && isspace((unsigned char)*end))
        end++;

    if (end == input || *end != '\0')
        return -1;

    if (value < 0 || value > 100000)
        return -1;

    return (int)value;
}


/* Select Radio or Ethernet */

void SelectInterface(InterfaceType interface)
{
    current_interface = interface;

    UpdateMulticastBuffer();

    if (interface == RADIO)
        printf("\nRadio interface selected.\n");

    else if (interface == ETHERNET)
        printf("\nEthernet interface selected.\n");
}


/* Get active entry count */

int GetCurrentCount(void)
{
    if (current_interface == RADIO)
        return Radio_count;

    if (current_interface == ETHERNET)
        return Ethernet_count;

    return 0;
}


/* Remaining capacity */

int GetRemainingCapacity(void)
{
    return MAX_ENTRIES - GetCurrentCount();
}


/* Check duplicate */

bool IsDuplicate(const char *data)
{
    int i;

    if (current_interface == RADIO)
    {
        for (i = 0; i < Radio_count; i++)
        {
            if (strcmp(Radio_entries[i], data) == 0)
                return true;
        }
    }

    else if (current_interface == ETHERNET)
    {
        for (i = 0; i < Ethernet_count; i++)
        {
            if (strcmp(Ethernet_entries[i], data) == 0)
                return true;
        }
    }

    return false;
}


/* Add entry into active buffer */

bool AddEntry(const char *data)
{
    if (current_interface == NONE)
    {
        printf("\nSelect Radio or Ethernet first.\n");
        return false;
    }

    if (data == NULL || data[0] == '\0')
    {
        printf("\nEmpty entry not allowed.\n");
        return false;
    }

    if (strchr(data, '|') != NULL)
    {
        printf("\nPipe character is reserved as separator.\n");
        return false;
    }

    if (strlen(data) >= MAX_DATA_LENGTH)
    {
        printf("\nEntry is too long.\n");
        return false;
    }

    /* Check maximum capacity */

    if (GetCurrentCount() >= MAX_ENTRIES)
    {
        printf("\nMaximum 5 entries allowed.\n");
        return false;
    }

    /* Check duplicate */

    if (IsDuplicate(data))
    {
        printf("\nDuplicate entry not allowed.\n");
        return false;
    }

    /* Add Radio entry */

    if (current_interface == RADIO)
    {
        strcpy(Radio_entries[Radio_count], data);
        Radio_count++;
    }

    /* Add Ethernet entry */

    else if (current_interface == ETHERNET)
    {
        strcpy(Ethernet_entries[Ethernet_count], data);
        Ethernet_count++;
    }

    UpdateMulticastBuffer();

    return true;
}


/* Store manual textfield input */

void SetManualInput(const char *data)
{
    snprintf(Input_buffer, sizeof(Input_buffer), "%s", data);
}


/* ADD BUTTON FUNCTION */

bool AddManualData(void)
{
    if (Input_buffer[0] == '\0')
    {
        printf("\nEnter data before clicking Add.\n");
        return false;
    }

    if (AddEntry(Input_buffer))
    {
        Input_buffer[0] = '\0';

        printf("\nManual data added successfully.\n");

        return true;
    }

    return false;
}


/* Update active multicast buffer */

void UpdateMulticastBuffer(void)
{
    int i;

    multicast_buffer[0] = '\0';

    if (current_interface == RADIO)
    {
        for (i = 0; i < Radio_count; i++)
        {
            if (i > 0)
                strcat(multicast_buffer, "|");

            strcat(multicast_buffer, Radio_entries[i]);
        }

        strcpy(Radio_buffer, multicast_buffer);
    }

    else if (current_interface == ETHERNET)
    {
        for (i = 0; i < Ethernet_count; i++)
        {
            if (i > 0)
                strcat(multicast_buffer, "|");

            strcat(multicast_buffer, Ethernet_entries[i]);
        }

        strcpy(Ethernet_buffer, multicast_buffer);
    }
}


/* MULTICAST BUTTON FUNCTION */

void OpenMulticastSelection(void)
{
    int i;

    if (current_interface == NONE)
    {
        printf("\nSelect an interface first.\n");
        return;
    }

    int remaining = GetRemainingCapacity();

    if (remaining == 0)
    {
        printf("\nBuffer already contains 5 entries.\n");
        return;
    }

    /* Sample address book data */

    const char *radio_list[] =
    {
        "1234",
        "45665",
        "1236",
        "565",
        "2333",
        "7890",
        "5678"
    };

    const char *ethernet_list[] =
    {
        "192.168.1.1",
        "192.168.1.2",
        "192.168.1.3",
        "192.168.1.4",
        "192.168.1.5",
        "192.168.1.6",
        "192.168.1.7"
    };

    if (current_interface == RADIO)
    {
        candidate_count = sizeof(radio_list) /
                          sizeof(radio_list[0]);

        for (i = 0; i < candidate_count; i++)
        {
            strcpy(candidates[i], radio_list[i]);
        }
    }

    else
    {
        candidate_count = sizeof(ethernet_list) /
                          sizeof(ethernet_list[0]);

        for (i = 0; i < candidate_count; i++)
        {
            strcpy(candidates[i], ethernet_list[i]);
        }
    }

    /* Reset temporary checkbox selections */

    for (i = 0; i < candidate_count; i++)
    {
        candidate_checked[i] = false;
    }

    printf("\n----------------------------------\n");
    printf("MULTICAST ADDRESS BOOK\n");
    printf("----------------------------------\n");

    printf("Existing entries : %d\n", GetCurrentCount());
    printf("Remaining slots  : %d\n", remaining);

    printf("\nAvailable data:\n");

    for (i = 0; i < candidate_count; i++)
    {
        printf("%d. %s", i + 1, candidates[i]);

        if (IsDuplicate(candidates[i]))
            printf(" [Already Added]");

        printf("\n");
    }

    printf("\nEnter checkbox number to select.\n");
    printf("Enter 0 when finished selecting.\n");

    while (1)
    {
        int choice;

        printf("\nSelect checkbox: ");
        choice = ReadNumber();

        if (choice == 0)
            break;

        if (choice < 1 || choice > candidate_count)
        {
            printf("Invalid selection.\n");
            continue;
        }

        i = choice - 1;

        if (IsDuplicate(candidates[i]))
        {
            printf("Already present in buffer.\n");
            continue;
        }

        if (candidate_checked[i])
        {
            candidate_checked[i] = false;

            printf("Unchecked: %s\n", candidates[i]);

            continue;
        }

        int selected = 0;

        int j;

        for (j = 0; j < candidate_count; j++)
        {
            if (candidate_checked[j])
                selected++;
        }

        if (selected >= remaining)
        {
            printf("Maximum available selections reached.\n");
            continue;
        }

        candidate_checked[i] = true;

        printf("Selected: %s\n", candidates[i]);
    }

    printf("\nSelection completed.\n");

    printf("Click Done to transfer selected data.\n");

    printf("1. Done\n");
    printf("2. Cancel\n");

    int option = ReadNumber();

    if (option == 1)
    {
        MulticastDone();
    }
    else
    {
        printf("\nMulticast selection cancelled.\n");
    }
}


/* MULTICAST CHECKBOX FUNCTION */

void MulticastCheckboxChanged(int index, bool checked)
{
    if (index < 0 || index >= candidate_count)
        return;

    if (IsDuplicate(candidates[index]))
        return;

    int selected = 0;
    int i;

    for (i = 0; i < candidate_count; i++)
    {
        if (candidate_checked[i])
            selected++;
    }

    if (checked && !candidate_checked[index])
    {
        if (selected >= GetRemainingCapacity())
        {
            printf("\nMaximum selection capacity reached.\n");
            return;
        }
    }

    candidate_checked[index] = checked;
}


/* DONE BUTTON FUNCTION */

void MulticastDone(void)
{
    int i;
    int added = 0;

    for (i = 0; i < candidate_count; i++)
    {
        if (candidate_checked[i])
        {
            if (AddEntry(candidates[i]))
                added++;
        }
    }

    UpdateMulticastBuffer();

    printf("\n%d multicast entries added.\n", added);

    DisplayBuffer();
}


/* Display current buffer */

void DisplayBuffer(void)
{
    UpdateMulticastBuffer();

    printf("\n----------------------------------\n");

    if (current_interface == RADIO)
        printf("RADIO MULTICAST BUFFER\n");

    else if (current_interface == ETHERNET)
        printf("ETHERNET MULTICAST BUFFER\n");

    printf("----------------------------------\n");

    printf("%s\n", multicast_buffer);

    printf("Total entries: %d / 5\n", GetCurrentCount());

    printf("----------------------------------\n");
}


/* MAIN FUNCTION */

int main(void)
{
    int choice;

    char manual_data[MAX_DATA_LENGTH];

    while (1)
    {
        printf("\n\n========== MAIN MENU ==========\n");

        printf("1. Select Radio\n");
        printf("2. Select Ethernet\n");
        printf("3. Enter Manual ID/IP\n");
        printf("4. Add Manual Data\n");
        printf("5. Multicast Button\n");
        printf("6. Display Multicast Buffer\n");
        printf("7. Exit\n");

        printf("\nEnter your choice: ");

        choice = ReadNumber();

        switch (choice)
        {
            case 1:

                SelectInterface(RADIO);

                break;

            case 2:

                SelectInterface(ETHERNET);

                break;

            case 3:

                printf("\nEnter Radio ID / Ethernet IP: ");

                ReadLine(manual_data, sizeof(manual_data));

                SetManualInput(manual_data);

                printf("Input_buffer = %s\n", Input_buffer);

                break;

            case 4:

                AddManualData();

                DisplayBuffer();

                break;

            case 5:

                OpenMulticastSelection();

                break;

            case 6:

                DisplayBuffer();

                break;

            case 7:

                printf("\nProgram terminated.\n");

                return 0;

            default:

                printf("\nInvalid choice.\n");

                break;
        }
    }

    return 0;
}
