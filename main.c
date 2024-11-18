#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

//defining the variables, structures and other data storing types

struct Date
{
    int day, month, year;
};

struct Transaction
{
    int accountID;
    struct Date transactionDate;
    char type[50];
    float amount;
    char entityName[100];
};
struct Transaction transactions[500];
unsigned int numberOfTransactions;

struct Account
{
    int id;
    char type[50];
    float balance;
    struct Transaction accountTransactions[100];
    int nrOfAccountTransactions;
};
struct Account accounts[50];
unsigned int numberOfAccounts;

FILE *filePointer;
const char* csvAccountsFileName = "accounts.csv";
const char* csvTransactionsFileName = "transactions.csv";


//getters and setters

float getBalance(int nr)
{
    return accounts[nr].balance;
}

int getID(int nr)
{
    return accounts[nr].id;
}

const char* getType(int nr)
{
    return accounts[nr].type;
}

void setBalance(int nr, float newBalance)
{
    accounts[nr].balance = newBalance;
}


//functions

float readAmount()
{
    float amount;
    scanf("%f", &amount);
    return amount;
}

int readID()
{
    int id;
    scanf("%d", &id);
    return id;
}

int existingID(int id)
{
    for(int i=0;i<=numberOfAccounts;i++)
        if(accounts[i].id == id)
            return i;  // if the id is found, the function returns the position on which the account is stored
    return -1;
}

//account managing
void createAccount()
{
    if(numberOfAccounts < 100)
    {

        printf("What is the type of the account you would like to create? (savings, credit, checkings)\n-> ");
        int sem = 0, first = 0;
        char type[11] = "";
        while (sem==0)
        {
            if(first==0)
                getchar();  // ignoring the newline if necessary (the first time that the loop is entered)
            fgets(type,11,stdin);
            first = 1;
            type[strlen(type)-1]='\0';  // remove the newline from the input of the user
            if( strcmp(type,"savings")==0 || strcmp(type,"credit")==0 || strcmp(type,"checkings")==0 )
            {
                strcpy(accounts[numberOfAccounts].type, type);
                sem = 1;
            }
            else
            {
                printf("\nPlease enter <<savings>>, <<credit>>, or <<checkings>>: ");
                sem = 0;
            }
        }

        printf("\nPlease enter the initial balance of the account: ");
        accounts[numberOfAccounts].balance = readAmount();

        int id = rand();
        while(existingID(id) != -1)
            id = rand();
        accounts[numberOfAccounts].id = id;

        numberOfAccounts += 1;
    }
}

int editAccount()
{
    printf("\nPlease enter the id of the account you want to edit: ");
    int id = readID();
    if(existingID(id) == -1)  // if the id is not found, the existingID function returns -1
    {
        printf("\nError: The account you were searching for could not be found.");
        return 0;
    }
    else
    {
        printf("\nWhat is the new balance of the account? ");
        float newBalance = readAmount();
        int accountNumber = existingID(id);  // we get the position of the account to edit in our array "accounts"
        setBalance(accountNumber, newBalance);  // we edit the account
    }
    return 1;
}

int deleteAccount()
{
    printf("\nPlease enter the id of the account you want to delete: ");
    int id = readID();
    if(existingID(id) == -1)
    {
        printf("\nError: The account you were searching for could not be found.");
        return 0;
    }
    else
    {
        int accountNumber = existingID(id);
        if(accountNumber == 0)
        {
            if(strcmp(accounts[0].type,"")==0)
            {
                printf("\nError: The account you were searching for could not be found.");
                return 0;
            }
            else
            {
                accounts[0].id = 0;
                strcpy(accounts[0].type,"");
                accounts[0].balance = 0;
            }
        }
        else
            for(int i=accountNumber; i<numberOfAccounts; i++)
                accounts[i] = accounts[i+1];
    }
    return 1;
}

//transactions recording
struct Date readAndValidateDate()
{
    int day, month, year;
    char aux[10]="";
    printf("day: ");
    int sem=0;
    while(!sem)  // while the introduced day is not valid
    {
        getchar();
        fgets(aux,3,stdin);  // reading the first two digits of what the user types from the console
        sem=1;
        day = atoi(aux);  // converting the number from string to int
        if(day > 31 || day <= 0)
        {
            printf("\nPlease enter a valid date: ");
            fseek(stdin,0,SEEK_END);
            sem = 0;
        }
    }

    memset(aux,0,strlen(aux)); fseek(stdin,0,SEEK_END);  // reseting the aux variable so we can reuse it and stdin so we don't have any leftover values from the previous reading
    printf("month: ");
    sem = 0;
    while(!sem)
    {
        fgets(aux,3,stdin);
        sem=1;
        month = atoi(aux);
        if(month > 12 || month <= 0)
        {
            printf("\nPlease enter a valid date: ");
            fseek(stdin,0,SEEK_END);
            sem = 0;
        }
    }

    memset(aux,0,strlen(aux)); fseek(stdin,0,SEEK_END);
    printf("year: ");
    sem = 0;
    while(!sem)
    {
        fgets(aux,5,stdin);  // reading the first 4 digits from the input of the user
        sem=1;
        year = atoi(aux);
        if(year > 2040 || year <= 2000)
        {
            printf("\nPlease enter a valid date: ");
            fseek(stdin,0,SEEK_END);
            sem = 0;
        }
    }
    struct Date date;
    date.day = day;
    date.month = month;
    date.year = year;
    return date;
}

int receiveTransfer(float amount)
{
    printf("\nWhat is the account you want to make the transfer to? (enter the id of the account): ");
    int found = 0;
        while(!found)
        {
            int id = readID();
            if(id == 0)
                return 0;
            if(existingID(id) == -1)
                printf("\nError: The account you were searching for could not be found. Try again or press 0 to abort operation.");
            else
            {
                int accountNumber = existingID(id);
                accounts[accountNumber].balance += amount;  // we change the balance of the account associated to the transaction
                accounts[accountNumber].nrOfAccountTransactions += 1;
                return 1;
            }
        }
    return 0;
}

void getEntityInformation()
{
    char aux[100]="";
    printf("\nWho was this payment made to? (enter the name of the entity): ");
    getchar();
    fgets(aux,100,stdin);
    aux[strlen(aux)-1]='\0';
    strcpy(transactions[numberOfTransactions].entityName, aux);
}

int recordTransaction()
{
    if(numberOfTransactions<500)
    {
        printf("\nPlease enter the amount of the transaction: ");
        transactions[numberOfTransactions].amount = readAmount();

        printf("\nPlease enter the date of the transaction you want to record: \n");
        transactions[numberOfTransactions].transactionDate = readAndValidateDate();

        printf("\nWhat is the account associated with this transaction? (enter the id of the account): ");
        int found = 0;
        while(!found)
        {
            int id = readID();
            if(id == 0)
                return 0;
            if(existingID(id) == -1)
                printf("\nError: The account you were searching for could not be found. Try again or press 0 to abort operation.");
            else
            {
                int accountNumber = existingID(id);
                transactions[numberOfTransactions].accountID = accounts[accountNumber].id;
                printf("What is the type of the transaction you would like to record? (deposit, withdrawal, payment, transfer)\n-> ");
                int sem = 0, first = 0;
                char type[12] = "";
                while (sem==0)
                {
                    if(first==0)
                        getchar();  // ignoring the newline if necessary (the first time that the loop is entered)
                    fgets(type,11,stdin);
                    first = 1;
                    type[strlen(type)-1]='\0';  // remove the newline from the input of the user
                    if(strcmp(type,"deposit")==0 )
                    {
                        accounts[accountNumber].balance += transactions[numberOfTransactions].amount;
                        accounts[accountNumber].nrOfAccountTransactions += 1;
                        numberOfTransactions += 1;
                        return 1;
                    }
                    else
                    {
                        if(transactions[numberOfTransactions].amount > accounts[accountNumber].balance)
                        {
                            printf("\nError: not enough money in the account to make the transaction! ");
                            return 0;
                        }
                        else
                        {
                            if(strcmp(type,"withdrawal")==0)
                            {
                                accounts[accountNumber].balance -= transactions[numberOfTransactions].amount;
                                accounts[accountNumber].nrOfAccountTransactions += 1;
                                numberOfTransactions += 1;
                                return 1;
                            }
                            else
                                if(strcmp(type, "payment")==0)
                                {
                                    accounts[accountNumber].balance -= transactions[numberOfTransactions].amount;
                                    accounts[accountNumber].nrOfAccountTransactions += 1;
                                    numberOfTransactions += 1;
                                    getEntityInformation();
                                    return 1;
                                }
                                else
                                    if(strcmp(type,"transfer")==0)
                                    {
                                        int received = receiveTransfer(transactions[numberOfTransactions].amount);
                                        if(received)
                                        {
                                            accounts[accountNumber].balance -= transactions[numberOfTransactions].amount;
                                            accounts[accountNumber].nrOfAccountTransactions += 1;
                                            numberOfTransactions += 1;
                                            return 1;
                                        }
                                        else
                                            return 0;
                                    }
                                    else
                                    {
                                        printf("\nPlease enter <<deposit>>, <<withdrawal>>, <<payment>> or <<transfer>>: ");
                                        sem = 0;
                                    }
                        }
                    }
                }
            }
        }
    }
    else
        return 0;
}

int calculateAccountBalance()
{
    printf("\nPlease enter the id of the account you want to check: ");
    int id = readID();
    if(existingID(id) == -1)
    {
        printf("\nError: The account you were searching for could not be found.");
        return 0;
    }
    else
    {
        int accountNumber = existingID(id);
        printf("\nThe balance of the account is %.2f: ", accounts[accountNumber].balance);
    }
    return 1;
}

//financial reports
int compareDates(struct Date date1, struct Date date2)
{
    if(date1.year < date2.year)
        return 1;
    else
    {
        if(date1.year > date2.year)
            return 0;
        else
        {
            if(date1.month < date2.month)
                return 1;
            else
            {
                if(date1.month > date2.month)
                    return 0;
                else
                {
                    if(date1.day < date2.day)
                        return 1;
                    else
                    {
                        if(date1.day > date2.day)
                            return 0;
                        else
                            return 1;
                    }
                }
            }
        }
    }
}

void generateAccountStatement()
{
    printf("\nPlease enter the id of the account for which you want to generate the statement: ");
    int id = readID();
    if(existingID(id) == -1)
    {
        printf("\nError: The account you were searching for could not be found.");
        return;
    }
    else
    {
        struct Date start_date, end_date;
        printf("\nPlease enter the start date of the statement: ");
        start_date = readAndValidateDate();
        printf("\nPlease enter the end date of the statement: ");
        end_date = readAndValidateDate();
        printf("%-20s %-20s %-20s %-20s %-20s\n","Transaction number","Type","Date","Amount","Entity");
        int accountNumber = existingID(id);
        for(int i=0; i<accounts[accountNumber].nrOfAccountTransactions; i++)
        {
            if(compareDates(start_date, accounts[accountNumber].accountTransactions[i].transactionDate) && compareDates(accounts[accountNumber].accountTransactions[i].transactionDate, end_date))
            {
                char type[50]="",entity[100]="";
                strcpy(type, accounts[accountNumber].accountTransactions[i].type);
                int day = accounts[accountNumber].accountTransactions[i].transactionDate.day;
                int month = accounts[accountNumber].accountTransactions[i].transactionDate.month;
                int year = accounts[accountNumber].accountTransactions[i].transactionDate.year;
                float amount = accounts[accountNumber].accountTransactions[i].amount;
                printf("%-20d %-20s %-2d.%-2d.%-14d %-20.2f", i, type, day, month, year, amount);
                if(strcmp(type,"payment")==0)
                {
                    strcpy(entity, accounts[accountNumber].accountTransactions[i].entityName);
                    printf("%-20s", entity);
                }
                printf("\n");
            }
        }
    }
}

void transactionsRegister()
{
    printf("%-20s %-20s %-20s %-20s %-20s %-20s\n","Transaction number","Type","Account ID","Date","Amount","Entity");
    for(int i=0; i<numberOfTransactions; i++)
    {
        char type[50]="",entity[100]="";
        strcpy(type, transactions[i].type);
        int account = transactions[i].accountID;
        int day = transactions[i].transactionDate.day;
        int month = transactions[i].transactionDate.month;
        int year = transactions[i].transactionDate.year;
        float amount = transactions[i].amount;
        printf("%-20d %-20s %-20d %-2d.%-2d.%-14d %-20.2f", i+1, type, account, day, month, year, amount);
        if(strcmp(type,"payment")==0)
        {
            strcpy(entity, transactions[i].entityName);
            printf("%-20s", entity);
        }
        printf("\n");
    }
}

void printBalances()
{
    printf("%-20s %-20s\n", "Account ID", "Balance");
    for(int i=0; i<numberOfAccounts; i++)
        printf("%-20d %-20.2f\n", accounts[i].id, accounts[i].balance);
}

//import and export data from and to files
void readFromCSVFile()
{
    filePointer = fopen(csvAccountsFileName, "r");
    if(filePointer == NULL)
    {
        perror("\nThe accounts file could not be opened.");
        return;
    }
    else
    {
        char *p;
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), filePointer) != NULL)
        {

            p=strtok(buffer,",");
            for(int i=0;i<3;i++)
            {
                if(i==0)
                    accounts[numberOfAccounts].id = atoi(p);
                if(i==1)
                    strcpy(accounts[numberOfAccounts].type,p);
                if(i==2)
                    accounts[numberOfAccounts].balance = atof(p);
                p=strtok(NULL,",");

            }
            numberOfAccounts++;
        }
        fclose(filePointer);
    }

    filePointer = fopen(csvTransactionsFileName, "r");
    if(filePointer == NULL)
    {
        perror("\nThe transactions file could not be opened.");
        return;
    }
    else
    {
        char *p;
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), filePointer) != NULL)
        {

            p=strtok(buffer,",");
            for(int i=0;i<7;i++)
            {
                if(i==1)
                    strcpy(transactions[numberOfTransactions].type, p);
                if(i==2)
                    transactions[numberOfTransactions].accountID = atoi(p);
                if(i==3)
                    transactions[numberOfTransactions].transactionDate.day = atoi(p);
                if(i==4)
                    transactions[numberOfTransactions].transactionDate.month = atoi(p);
                if(i==5)
                    transactions[numberOfTransactions].transactionDate.year = atoi(p);
                if(i==6)
                    transactions[numberOfTransactions].amount = atof(p);
                p=strtok(NULL,",");
            }
            if(p != NULL)
                strcpy(transactions[numberOfTransactions].entityName, p);
            for(int i=0;i<numberOfAccounts;i++)
                if(transactions[numberOfTransactions].accountID == accounts[i].id)
                {
                    accounts[i].accountTransactions[accounts[i].nrOfAccountTransactions] = transactions[numberOfTransactions];
                    accounts[i].nrOfAccountTransactions++;
                    if(strcmp(transactions[numberOfTransactions].type,"deposit")==0)
                        accounts[i].balance += transactions[numberOfTransactions].amount;
                    else
                        accounts[i].balance -= transactions[numberOfTransactions].amount;
                }
            numberOfTransactions++;
        }
        fclose(filePointer);
    }
}

void saveDataCSVFile()
{
    filePointer = fopen(csvAccountsFileName, "w");
    if(filePointer == NULL)
    {
        perror("\nThe accounts file could not be opened.");
        return;
    }
    else
    {
        for(int i=0; i<numberOfAccounts; i++)
            fprintf(filePointer, "%d,%s,%.2f\n", accounts[i].id, accounts[i].type, accounts[i].balance);
    }
    fclose(filePointer);

    filePointer = fopen(csvTransactionsFileName, "w");
    if(filePointer == NULL)
    {
        perror("\nThe transactions file could not be opened.");
        return;
    }
    else
    {
        for(int i=0; i<numberOfTransactions; i++)
        {
            fprintf(filePointer, "%d,%s,%d,%d,%d,%d,%.2f", i, transactions[i].type, transactions[i].accountID, transactions[i].transactionDate.day, transactions[i].transactionDate.month, transactions[i].transactionDate.year, transactions[i].amount);
            if(strcmp(transactions[i].type, "payment")==0)
                fprintf(filePointer,",%s", transactions[i].entityName);
            fprintf(filePointer,"\n");
        }
    }
    fclose(filePointer);
}


int main()
{
    srand(time(NULL));
    // menu
    printf("\n----------Menu-------------"
    "\nPlease choose one of the following options:"
    "\n1. Create a new account."
    "\n2. Edit an existing account."
    "\n3. Delete an account."
    "\n4. Record a new transaction."
    "\n5. Calculate the balance of an account."
    "\n6. Generate account statements."
    "\n7. Show balances for all accounts."
    "\n8. Generate a transaction register."
    "\n9. Read data from a CSV file."
    "\n10. Save data in a CSV file."
    "\n0. Exit.");

    int option=-1;
    while(option!=0)
    {
        printf("\n\nEnter the desired option: "); scanf("%d",&option);
        switch(option)
        {
            case 1:
                {
                    createAccount();
                    printf("\nAccount successfully added.");
                    printf(" The ID of your new %s", getType(numberOfAccounts-1));
                    printf(" account is %d", getID(numberOfAccounts-1));
                    break;
                }
            case 2:
                {
                    int result = editAccount();
                    if(result)
                        printf("\nAccount successfully edited.");
                    break;
                }
            case 3:
                {
                    int result = deleteAccount();
                    if(result)
                        printf("\nAccount successfully deleted.");
                    break;
                }
            case 4:
                {
                    int result = recordTransaction();
                    if(result)
                        printf("\nTransaction successfully recorded.");
                    break;
                }
            case 5:
                {
                    calculateAccountBalance();
                    break;
                }
            case 6:
                {
                    generateAccountStatement();
                    break;
                }
            case 7:
                {
                    printBalances();
                    break;
                }
            case 8:
                {
                    transactionsRegister();
                    break;
                }
            case 9:
                {
                    readFromCSVFile();
                    break;
                }
            case 10:
                {
                    saveDataCSVFile();
                    break;
                }
            case 0:{ printf("\nEnd of the program."); break; }
            default: printf("\nPlease enter a valid option: ");
        }
    }

    return 0;
}
