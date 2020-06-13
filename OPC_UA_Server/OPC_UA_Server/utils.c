#include "utils.h"

/* loadFile parses the certificate file.
 *
 * @param  path               specifies the file name given in argv[]
 * @return Returns the file content after parsing */
UA_ByteString loadFile(const char *path) {

        UA_ByteString fileContents = UA_STRING_NULL;

        /* Open the file */
        FILE* fp = fopen(path, "rb");
        if (!fp) {
                errno = 0; /* We read errno also from the tcp layer... */
                return fileContents;
        }

        /* Get the file length, allocate the data and read */
        fseek(fp, 0, SEEK_END);
        fileContents.length = (size_t)ftell(fp);
        fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
        if (fileContents.data) {
                fseek(fp, 0, SEEK_SET);
                size_t read = fread(fileContents.data, sizeof(UA_Byte), fileContents.length, fp);
                if (read != fileContents.length)
                        UA_ByteString_clear(&fileContents);
        }
        else {
                fileContents.length = 0;
        }
        fclose(fp);

        return fileContents;
}

// controlla che l'usage sia del tipo: server.exe [--cert <pathCertificato> --key <pathChiave>] [--url <customUdpUadpUrl>] 
applicationConfig_t parseArgument(int argc, char* argv[]) {

    applicationConfig_t appConf = { false, true, NULL, NULL, NULL };

    if (argc == 1)
        return appConf;

    if (strncmp(argv[1], "-h", 2) == 0) {
        fprintf(stderr, "Usage: server.exe [--cert <pathCertificato> --key <pathChiave>] [--url <customUdpUadpUrl>]\n");
        exit(EXIT_SUCCESS);
    }

    for (int i = 1; i < argc - 1; i++) {
        if (strncmp(argv[i], "--cert", 6) == 0) {
            if (strncmp(argv[i + 2], "--key", 5) == 0) {
                appConf.encryption = true;
                appConf.certPath = argv[i + 1];
                appConf.keyPath = argv[i + 3];
                i += 3;
            }
        }
        else if (strncmp(argv[i], "--url", 5) == 0) {
            if (strncmp(argv[i + 1], "opc.udp://", 10) == 0) {
                appConf.usingUdpUadp = true;
                appConf.customUrl = argv[i + 1];
                i += 1;
            }
        }
    }

    return appConf;
}

//Presentazione del server e visualizzazione della configurazione attuale
void printWelcome(const applicationConfig_t appConf) {
    printf("Welcome in OPC UA Server\n");
    printf("You are running this process with following config:\n");
    if (appConf.encryption) {
        printf("* Using Encryption\n");
        printf("    - Cert at %s\n", appConf.certPath);
        printf("    - Key at %s\n", appConf.keyPath);
    }
    if (appConf.usingUdpUadp) {
        printf("* Enabled PubSub with UDP UADP\n");
        if (appConf.customUrl != NULL) {
            printf("* UDP UADP custom url enabled\n");
            printf("    - Custom url %s\n", appConf.customUrl);
        }
        else {
            printf("    - Default url opc.udp://224.0.0.22:4840/\n");
        }
    }
    printf("\n");
}
