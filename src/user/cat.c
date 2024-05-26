#include "cat.h"

/**
    * Split string by first occurence of a character.
    * Example:
    *  pstr = "file.txt"
    * , by = '.'
    * , result = "file".
    * 
    * After the function is called, pstr will be "txt".
    * 
    * @param pstr string to be splitted
    * @param by character to split the string
    * @param result string to store the splitted string
 */
void split_by_first(char* pstr, char by, char* result)
{
  int i = 0;
  while (pstr[i] != '\0' && pstr[i] != by)
  {
    result[i] = pstr[i];
    i++;
  }
  result[i] = '\0';

  if (pstr[i] == by)
  {
    i++;
    int j = 0;
    while (pstr[i] != '\0')
    {
      pstr[j] = pstr[i];
      i++;
      j++;
    }
    pstr[j] = '\0';
  }
  else
  {
    *pstr = '\0';
  }
}

void cat(char argument[]) {
    put(argument, BIOS_BLACK);
    put("cat command\n", BIOS_LIGHT_GREEN);
    int32_t errorCode; 

    char temp_path[256];
    // temp_path = current_path;
    memcpy(temp_path, current_path, strlen(current_path));

    // Get the filename
    char filename[strlen(argument)];

    // Split the argument by '.' and make argument as the extension
    split_by_first(argument, '.', filename);

    // Check if the filename length is more than 8
    if (strlen(filename) > 8)
    {
        put("Filename must less than 8 characters\n", BIOS_RED);
        return;
    }

    uint8_t name_len = strlen(filename);
    global_request.buffer_size = CLUSTER_SIZE;
    global_request.buf = buf;

    // Fill the rest with null
    while (name_len < 8)
    {
        filename[name_len] = '\0';
          name_len++;
    }

    // Copy the filename and extension
    memcpy(global_request.name, filename, name_len);
    memcpy(global_request.ext, argument, strlen(argument));

    
    global_request.parent_cluster_number = current_directory;
    // read_syscall(global_request, &errorCode);
    syscall(0, (uint32_t) &global_request, (uint32_t) &errorCode, 0);   // ga tau bnr apa ga, cobain ajalah
    memcpy(current_path, temp_path, strlen(temp_path));
    if (errorCode == 0)
    {
        // put(global_request.buf, strlen(global_request.buf), 0xF);
        // put("\n", 1, 0xF);
        put((char*) global_request.buf, BIOS_WHITE);
    }
    else if (errorCode == 1)
    {
        // put("Not a file\n", 11, 0x4);
        put("Not a file\n", BIOS_RED);
    }
    else if (errorCode == 2)
    {
        // put("Not enough buffer\n", 18, 0x4);
        put("Not enough buffer\n", BIOS_RED);
    }
    else if (errorCode == -1)
    {
        // put("Unknown error -1\n", 17, 0x4);
        put("Unknown error: -1\n", BIOS_RED);
    }
    else
    {
        // put("Unknown error\n", 14, 0x4);
        put("Unknown error\n", BIOS_RED);
    }
}

