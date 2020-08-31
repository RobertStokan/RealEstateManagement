//*****************************************************************************
// FILENAME: RealEstateTracker.CPP
// DESCRIPTION: This program will maintain records of real estate listings 
// DESIGNER: Robert Stokan
// FUNCTIONS: readFile - Reads input file into dynamically allocated linked list 
// displayAll - Displays all listings currently in linked list  
// AddListing - Allows user to manually add new listing(s) to list  
// ValidateMLS - Validates format of MLS number entered by user
// ValidatePrice - Validates price entered by user 
// ValidateZip - Validates format of zip code entered by user 
// ValidateStatus - Validates status entered by user 
// ValidateCompanyName - Validates format of company name entered by user
// DeleteRecord - Allows user to remove listing(s) from the list  
// SaveToFile - Allows user to save changes to the file before exiting program  
// ChangeAskingPrices - Allows user to apply price changes from file 
//*****************************************************************************  

#include <iostream>         // for I/O
#include <iomanip>          // for formatting output
#include <cstdlib>          // for using PAUSE statement
#include <string>           // for using string variable 
#include <fstream>          // for file I/O 

using namespace std;


// constants 
const char YES = 'Y'; 							// "Yes" response to user prompt
const char NO = 'N'; 							// "No" response to user prompt 
const string FILE_NAME = "LISTINGS.TXT"; 		// Input/Output file name
const string FILE_CHANGES = "CHANGES.TXT"; 		// Name of changes file  
const int ZIP_CODE_LENGTH = 10;					// Length of zip code  
const int COMPANY_LENGTH = 20; 					// Maximum length of company name 
const char FILE_CHAR = 'F'; 					// Character to enter another file name 
const char MENU_CHAR = 'M'; 					// Character to return to action menu 
const int MAX_PER_LINE = 7; 					// To regulate MLS numbers displayed per line  
const char EXISTING_FILE = 'E'; 				// Character to overwrite existing file 
const char ANOTHER_FILE = 'A'; 					// Character to choose another file 
const int MLS_MAX = 999999; 					// Maximum size of MLS number 
const int MLS_MIN = 100000; 					// Minimum size of MLS number 


// enumerated data type
enum statusOptions {AVAILABLE, CONTRACT, SOLD};    // Enumerated data type for listing status 


// struct 
struct listingsInfo			    // Struct to store listing data in linked list  
{
	int numberMLS;				// MLS number
	double price;				// Listing price 
	statusOptions status; 		// Listing status 
	string zipCode;				// Zip Code 
	string realtyCompany; 		// Realty company for listing 
	listingsInfo *link; 		// Link to next node in linked list 
	
}; 


// Function prototypes
void readFile(ifstream& file, bool& exists, listingsInfo* &first, listingsInfo* &last); 
void displayAll(listingsInfo* first, listingsInfo* last);
void AddListing(listingsInfo* &first, listingsInfo* &last); 
int ValidateMLS();
double ValidatePrice();  
string ValidateZip(); 
statusOptions ValidateStatus(); 
string ValidateCompanyName(); 
void DeleteRecord(listingsInfo* &first, listingsInfo* last); 
void SaveToFile(ofstream& outputFile, listingsInfo* first);
void ChangeAskingPrices(listingsInfo* first, listingsInfo* last); 



//*****************************************************************************
// FUNCTION: main
// DESCRIPTION: Prompts user whether to open file, whether to proceed,
// provides menu options if user chooses to proceed, calls other functions
// to execute menu options.    
// CALLS TO: readFile, displayAll, AddListing, DeleteRecord, SaveToFile 
//*****************************************************************************  
int main()
{
	// variables 
	char loadData;				// For user input to load data 
	char proceed;  				// For user prompt to proceed with empty record 
	ifstream inputFile;			// Variable for input file 
	ofstream outputFile;		// Variable for output file   
	bool fileExists;			// To check if file exists 
	char menuOption;  			// To receive user input for menu option 
	
	listingsInfo *head; 		// To store first node in list 
	listingsInfo *last;			// To store last node in list 
	
	head = NULL;
	last = NULL;  
	
	
	// Program introduction
	cout << "This program maintains records of real estate listings" << endl << endl;  
	
	// Initial prompt to load file  
	do
	{
	
	   cout << "Load existing data from file (Y/N)?: "; 
	   cin >> loadData; 
	   cout << endl;  
	
	   loadData = toupper(loadData); 
	
	   if (loadData != YES && loadData != NO)
		   cout << endl<< "Invalid entry: Must be 'Y' or 'N'."
		   << endl; 
		   
	}
	while (loadData != YES && loadData != NO); 
	
	
	if (loadData == YES)
		readFile(inputFile, fileExists, head, last);
		

		do
		{
			cout << "Please choose from the options given below:" << endl << endl; 
			cout << "D - Display All Listings" << endl; 
			cout << "A - Add Listing" << endl; 
			cout << "R - Remove Listing" << endl;
			cout << "C - Apply Changes File" << endl; 
			cout << "E - Exit from Program" << endl << endl; 
	
			cout << "Enter selection: "; 
			cin >> menuOption; 
			cout << endl << endl; 
	
			menuOption = toupper(menuOption); 
	
		switch(menuOption)
		{
		case 'D':
			displayAll(head, last);
			break; 
		case 'A':
			AddListing(head, last);
			break; 
		case 'R': 
			DeleteRecord(head, last);
			break;
		case 'C':
			ChangeAskingPrices(head, last); 
			break; 
		case 'E':
			SaveToFile(outputFile, head);
			break; 
		default:
			cout << "Invalid Input - Must be from menu." << endl << endl; 	
		}
		
	}
	while(menuOption != 'E'); 
	
	
	
	system ("PAUSE"); 
	
	return 0; 
	
}

//*****************************************************************************
// FUNCTION: readFile
// DESCRIPTION: Reads input file data into linked list    
// INPUT: Parameters: file - variable for input file containing listing information  
// exists - Boolean variable to return whether file exists. 
// first - Pointer variable for first node in linked list 
// last - Pointer variable for last node in linked list  
// OUTPUT: reference parameters: file, exists, first, last  
//***************************************************************************** 
void readFile(ifstream& file, bool& exists, listingsInfo* &first, listingsInfo* &last)
{
	// function local variable
	int tempStatus; 		// to temporarily hold status digit while converting to enumerated type. 
	string fileName; 		// to receive user input for file name 
	char enterAnother; 		// to receive user choice for whether to enter another file name
	int tempMLS; 			// to store MLS outside of loop to use priming read 
	bool memoryFull; 		// to track when memory can no longer be allocated  
	 
	
	do
	{
	  // Asking user to enter input file name
	  cout << "Please enter the name of the input file: "; 
	  cin >> fileName; 
	  cout << endl;  
	
	  // to open the input file 
      file.open(fileName.c_str());
    
      // to check if input file exists 
      if (!file)
      {
         cout << "Error: input file not found." << endl << endl;
       
         do
         {
    		cout << "Do you wish to enter another file name ('F') or go to action menu ('M')?: ";
       		cin >> enterAnother; 
       		cout << endl; 
       
       		enterAnother = toupper(enterAnother); 
       
       		if (enterAnother != FILE_CHAR && enterAnother != MENU_CHAR)
       	  		cout << "Invalid input: Must be 'F' or 'M'. " << endl << endl; 
   	     }
   	     while (enterAnother != FILE_CHAR && enterAnother != MENU_CHAR); 
    
	     // To clear file stream variable 	
         if (enterAnother == FILE_CHAR)
	   	    file.clear(); 	
	   
	     exists = false; 
	    
    }
    else
       exists = true; 
    
	}
    while (exists == false && enterAnother != MENU_CHAR);   
    
    
    
    if (enterAnother != MENU_CHAR)
    {
    	
      	// Allocating memory for new node.   
      	listingsInfo *newNode; 
      
      	last = NULL; 
      
      	first = NULL; 
      
      	memoryFull = false; 
      	file >> tempMLS; 
    		
    	
      while (file && memoryFull == false)               
      {
      	
         newNode = new (nothrow) listingsInfo; 
         
         if(newNode == NULL)
         	memoryFull = true; 
         else
         {
         	
         	newNode->numberMLS = tempMLS;
		     	
      	 	file >> newNode->price; 
      	 
      	 
      	 	file >> tempStatus;
      	
 		 	newNode->status = static_cast<statusOptions>(tempStatus); 
		  
		 	file >> newNode->zipCode; 
		 
		 	getline(file, newNode->realtyCompany); 
		 
		 	newNode->realtyCompany.erase(0, 1);
		     	
		 	newNode->link = NULL; 
		 
		 	if (first == NULL)
		 	{
		 		first = newNode;
				last = newNode;  			  
		 	}
		 	else
		 	{
		 		last->link = newNode;
		 		last = newNode; 	
		 	}
		 
		 	file >> tempMLS; 
		 
		 }
	
	     exists = true;
	  
	  }
	   		 
    }
    
    
    file.close(); 
    	
}

//*****************************************************************************
// FUNCTION: displayAll
// DESCRIPTION: Formats and displays to screen all listings currently in list    
// INPUT: Parameters: first - Pointer variable for first node in linked list  
// last - Pointer variable for last node in linked list. 
// OUTPUT: Outputs list contents directly to screen.   
//***************************************************************************** 
void displayAll(listingsInfo* first, listingsInfo* last)
{
	
	// function local variables 
	int counter; 				// to track and control number of lines of output
	listingsInfo *current; 		// to hold the current node during each pass through loop 
	
		
	current = first;  
			
	
	if (current == NULL)
		cout << "There are no listings currently stored." << endl; 
	else
	{
		cout << right; 
		cout << setw(15) << "Asking" << setw(11) << "Listing" << endl; 
		cout << "MLS#" << setw(10) << "Price" << setw(11) << "Status" << setw(14) << "Zip Code" << setw(12) << "Realtor" << endl; 
		cout << "------" << setw(10) << "-------" << setw(12) << "---------" << setw(13) << "----------" << setw(15) << "------------" << endl; 
 		
		
		while (current != NULL)
		{
		
		cout << setprecision(0) << fixed; 
		
		
			cout << left; 
			cout << setw(10) << current->numberMLS
       		     << setw(9) << current->price; 
       		
			cout << setw(12);         
       	    switch(current->status) 
       	    {
       	    case AVAILABLE:
			   cout << "Available"; 
			   break;
			case CONTRACT:
			   cout << "Contract";
			   break;
			case SOLD:
			   cout << "Sold"; 
			   break;  
       	    }
       	    
       	    cout << setw(13) << current->zipCode
      	    	 << current->realtyCompany; 
				   
			current = current->link;	    
       	    	 
      	    cout << endl; 
		}
		
	cout << endl; 	
		
	}	
	
	
}

//*****************************************************************************
// FUNCTION: AddListing
// DESCRIPTION: Allows user to add new listings to linked list.    
// INPUT: Parameters: first - Pointer variable for first node in linked list  
// last - Pointer variable for last node in linked list. 
// OUTPUT: reference parameters: first, last 
// CALLS TO: ValidateMLS, ValidatePrice, ValidateZip, ValidateStatus, 
// ValidateCompanyName 
//***************************************************************************** 
void AddListing(listingsInfo* &first, listingsInfo* &last)
{
	char continueOption;       // For user prompt to add another listing 
	listingsInfo *newNode; 	   // Pointer variable for new node
	
	do
	{
		 
		 // To allocate memory for new node 
		 newNode = new (nothrow) listingsInfo; 

		 if(newNode == NULL)
		 {
		 	cout << "Memory is full. No more listings can be added." << endl << endl; 
		 	
		 	continueOption = NO; 
		 	
		 }
		 else
		 {
		 
	        // Function call to validate MLS number 
	        newNode->numberMLS = ValidateMLS();
	
	        // Function call to validate price 
	        newNode->price = ValidatePrice(); 
	     
	        // Function call to validate status 
            newNode->status = ValidateStatus(); 
         
            // Function call to validate zip code 
	        newNode->zipCode = ValidateZip(); 
         
            // Function call to validate zip code 
	        newNode->realtyCompany = ValidateCompanyName(); 
	     
	        newNode->link = NULL; 
	     
	     
	        if (first == NULL)
	        {
	     	   first = newNode;
	     	
	     	   last = newNode;	
	     	
	        }
	        else
	        {
	     	   last->link = newNode;
	     	
	     	   last = newNode; 
	     	
	        }
	     
	 	}
	  
	
		 if(newNode != NULL)
	     	do
	     	{
	     		cout << "Do you wish to add another listing (Y/N)?: ";
	     		cin >> continueOption; 
	     		cout << endl << endl; 
	
	     		continueOption = toupper(continueOption); 
	
	     		if (continueOption != YES && continueOption != NO)
		     		cout << "Invalid Input: Must be 'Y' or 'N'." << endl << endl; 
	     	}
	     while(continueOption != YES && continueOption != NO); 
	
    }
	while(continueOption != NO);
	
}


//*****************************************************************************
// FUNCTION: ValidateMLS
// DESCRIPTION: Validates formatting of MLS number input by user.    
// INPUT: Prompts for input directly from user. 
// OUTPUT: Return value: inputMLS - validated MLS number 
//***************************************************************************** 
int ValidateMLS()
{
	// Local variable 
	int inputMLS; 		// To receive MLS number input by user 
	
		
		while (inputMLS < MLS_MIN || inputMLS > MLS_MAX)
		{
		
		cout << "Please enter MLS number: ";
		cin >> inputMLS;
		cout << endl; 
		
		
		if (inputMLS < MLS_MIN)
		cout << "Invalid input - Number entered is too short" << endl
		<< "Must be 6 digits long and first digit cannot be '0'." << endl << endl;
		
		if (inputMLS > MLS_MAX)
		cout << "Invalid input - Number entered is too long." << endl
		<< "Must be 6 digits long and first digit cannot be '0'." 
		<< endl << endl; 
		
		}
		
	return inputMLS;  
	
}

//*****************************************************************************
// FUNCTION: ValidatePrice
// DESCRIPTION: Validates that price entered by user is greater than "0.00".     
// INPUT: Prompts for input directly from user. 
// OUTPUT: Return value: price - validated price of listing 
//***************************************************************************** 
double ValidatePrice()
{
	// local variable 
	double price = 0.00;		// To receive price from user 
	
	while (price <= 0.00)
	{
	cout << "Please enter price of listing: ";
	cin >> price; 
	cout << endl; 
	
	if (price <= 0.00)
		cout << "Price must be greater than $0.00. Try again." << endl; 
	}
	
	return price; 
	
}

//*****************************************************************************
// FUNCTION: ValidateZip
// DESCRIPTION: Validates formatting of zip code input by user.    
// INPUT: Prompts for input directly from user.
// OUTPUT: Return value: zipCodeInput - validated zip code.  
//***************************************************************************** 
string ValidateZip()
{
	string zipCodeInput; 		// Zip code input by user 
	int index; 					// Index position of character during each loop pass 
	bool validInput; 			// To store whether input is valid 
	
	
	do 
	{
	cout << "Please enter Zip Code for listing: ";
	cin >> zipCodeInput;
	cout << endl; 
	
	validInput = true; 
	  
	if (zipCodeInput.length() > ZIP_CODE_LENGTH)
	{
	   cout << "Input too long: must be 10 characters."; 
	   validInput = false; 
	   cout << endl << endl;
    }
    
    if (zipCodeInput.length() < ZIP_CODE_LENGTH)
	{
	   cout << "Input too short: must be 10 characters."; 
	   validInput = false; 
	   cout << endl << endl;
    }
    
    if (zipCodeInput[5] != '-')
    {
    	cout << "6th character of zip code must be '-'."; 
    	validInput = false;
    	cout << endl << endl; 
			
    }
	
	for(index = 0; index < zipCodeInput.length(); index++) 
	{
		if(!isdigit(zipCodeInput[index]) && index != 5)
		{
		    cout << "Only digits are allowed.";    
		    validInput = false;
			cout << endl << endl;   
		}
	} 
	
	   
	}
	while (!validInput); 
	
	return zipCodeInput; 
	
}

//*****************************************************************************
// FUNCTION: ValidateStatus
// DESCRIPTION: Validates status input by user and stores value as enumerated
// data type value.     
// INPUT: Prompts for input directly from user.
// OUTPUT: Return value: status - validated status  
//***************************************************************************** 
statusOptions ValidateStatus()
{
	statusOptions status; 		// To store status as enumerated type 
	char inputStatus; 			// To receive user input for status 
	bool validInput; 			// To store if input is valid 
	
	cout << "Please choose listing status from the options below: " << endl << endl; 
	
	cout << "'A' - AVAILABLE" << endl;
	cout << "'C' - CONTRACT" << endl;
	cout << "'S' - SOLD" << endl << endl;  
	
	do
	{
	cout << "Enter status: ";
	cin >> inputStatus;
	cout << endl; 
	
	inputStatus = toupper(inputStatus);
	validInput = true;
	
	switch (inputStatus)
	{
	case 'A':
		status = AVAILABLE; 
		break;
	case 'C':
		status = CONTRACT;
		break; 
	case 'S':
		status = SOLD;
		break; 
	default:
		validInput = false;
		cout << "Invalid input - Must be 'A', 'C', or 'S'." << endl 
		     << "Please choose again." << endl << endl; 
    }
	}
	while(validInput == false);  
	
	return status; 
	
	
}

//*****************************************************************************
// FUNCTION: ValidateCompanyName
// DESCRIPTION: Validates formatting of company name input by user.     
// INPUT: Prompts for input directly from user. 
// OUTPUT: Return value: companyName - validated company name.  
//*****************************************************************************  
string ValidateCompanyName()
{
	string companyName; 		// To store company name input by user
	int index; 					// To store index of character being checked/formatted 
	int lengthOfName;			// To store length of name input 
	bool validCharacter;		// To record if character is valid 
	   
	
	cin.ignore();
	
	// To validate that only letters and spaces have been entered and that 
	// length (including spaces) is 20 characters or less 
	do 
	{
		
	cout << "Please enter the Realty Company Name: "; 
	getline(cin, companyName); 
	cout << endl << endl; 
	
	if (companyName.length() > COMPANY_LENGTH)
		cout << "Input too long - must be 20 characters or less (including spaces)" << endl << endl;
		
	lengthOfName = companyName.length();
	validCharacter = true;
	
	for(index = 0; index < lengthOfName; index++) 
	{
		if(!isspace(companyName[index]) && !islower(companyName[index]) && !isupper(companyName[index]))
		{
    		cout << "Invalid input - Only letters and spaces are allowed" << endl << endl;
    		validCharacter = false; 
    	}	
    	
	}	
			
	}
	while(companyName.length() > COMPANY_LENGTH || validCharacter == false); 
	
	
	// To format user input correctly into uppercase and lowercase characters 
	index = 0; 
	companyName[index] = toupper(companyName[index]); 
	
	for(index = 1; index < lengthOfName; index++) 
	{
		if(isspace(companyName[index - 1]))
			companyName[index] = toupper(companyName[index]); 
		else
		  	companyName[index] = tolower(companyName[index]); 	
		
	}
	
	
	return companyName; 
	
}

//*****************************************************************************
// FUNCTION: DeleteRecord
// DESCRIPTION: Allows user to delete listing from linked list.    
// INPUT: Parameters: first - Pointer variable for first node in linked list
// last - Pointer variable for last node in linked list.   
// OUTPUT: reference parameter: first 
//***************************************************************************** 
void DeleteRecord(listingsInfo* &first, listingsInfo* last)
{
	
	// variables		
	int lineCounter = 0;		// Counter to control number of MLS numbers per line
	int mlsToSearch; 			// To receive input from user 
	bool found; 				// To track whether node node to delete has been found  
	listingsInfo *current; 		// To hold current node in loop to display MLS numbers to screen
	listingsInfo *searchNode;	// To hold current node in loop to search for Node to delete 
	listingsInfo *previous; 	// To hold previous node in loop to search for Node to delete 
		
	current = first; 
	
	if(current == NULL)
		cout << "There are no records currently on file." << endl << endl; 
	else
	{
	    
	   cout << "Please select MLS number from the choices below:" << endl << endl;   
	
	   while(current != NULL)
	   {
	      lineCounter++; 
		
		   if (lineCounter < MAX_PER_LINE)
			   cout << current->numberMLS << " "; 
		   else
		   { 
			   cout << current->numberMLS << endl;
			
			   lineCounter = 0; 
		   }
				
		   current = current->link;
	   }
	   
	   cout << endl << endl; 
	   
	   // Function call to validate MLS number to search 
	   mlsToSearch = ValidateMLS();  
	   
	   // To set found to false prior to search 
	   found = false; 
	   
	   searchNode = first; 
	   
	   while (searchNode != NULL && !found)
	   		if (searchNode->numberMLS == mlsToSearch)
	   			found = true; 
	   		else 
	   		{
	   			previous = searchNode; 
	   			searchNode = searchNode->link;	   
			}
	
	   if (!found)
	   	  cout << "Listing not found in records." << endl << endl;
		
		 	 
	
	   if (found)
	   {
	   	
	   		if(searchNode == first && first == last)
	   			first = NULL; 
	   		else if (searchNode == first && first != last)
	   		{
	   			first = searchNode->link; 
	   			
	   			delete searchNode; 
	   			
	   		}
	   		else
	   		{
	   	
	   		previous->link = previous->link->link; 
	   		
	   		delete searchNode;
			   
			}
	   
	   
	   cout << "The listing for MLS Number " << mlsToSearch << " has been deleted." << endl << endl;
	   
	   }  
	     
	}	
	
}

//*****************************************************************************
// FUNCTION: SaveToFile
// DESCRIPTION: Allows user to save changes to file before exiting program.    
// INPUT: Parameters: outputFile - variable for output file to save changes 
// first - Pointer variable for first node in linked list 
// OUTPUT: reference parameters: outputFile, first 
//***************************************************************************** 
void SaveToFile(ofstream& outputFile, listingsInfo* first)
{
	// variable 
	int counter;			// To provide index during loop for output to file 
	char saveOption;		// For user input to save changes or not 
	char confirm;   		// To confirm if user wishes to exit 
	string fileName;		// To receive user input for file name 
	char fileOption; 		// To recieve user confirmation to write over file 
	ifstream testFile; 		// To open file as an ifstream to test if it already exists.
	listingsInfo *current;  // To track current node during output loop   
	
	do
	{
	do
	{
	cout << "Do you wish to save changes to the file before exiting (Y/N)?: "; 
	cin >> saveOption; 
	cout << endl; 
	
	saveOption = toupper(saveOption); 
	
	
	if (saveOption != YES && saveOption != NO) 
		cout << "Invalid Input - Must be 'Y' or 'N'" << endl << endl;
	}
	while (saveOption != YES && saveOption != NO); 
	
	
	if(saveOption == YES)
	{
		do
		{
		cout << "Please enter the name of the file to which to save: "; 
		cin >> fileName; 
		cout << endl; 
		
		fileOption = EXISTING_FILE; 
		
		testFile.open(fileName.c_str());
		
		
		if (testFile)
		{
			cout << "File already exists." << endl;  
			cout << "Do you wish to overwrite existing file ('E') or choose another file ('A')?" 
			<< endl << endl; 
			
			do
			{
				cout << "Please enter choice: "; 
				cin >> fileOption; 
				cout << endl; 
			
				fileOption = toupper(fileOption); 
			
				if (fileOption != EXISTING_FILE && fileOption != ANOTHER_FILE) 
					cout << "Invalid Input - Must be 'E' or 'A'" << endl << endl;
			}
			while (fileOption != EXISTING_FILE && fileOption != ANOTHER_FILE); 
			
		}
		
		testFile.close();
		testFile.clear(); 	
		
		}
		while(fileOption == ANOTHER_FILE); 
		
		
		if(fileOption == EXISTING_FILE)
		{
			outputFile.open(fileName.c_str());
			
			
			current = first; 
			
			while(current != NULL)
			{
				outputFile << fixed << setprecision(0)
			   	   		   << current->numberMLS << " " 
			   	           << current->price << " "
			               << current->status << " "
			               << current->zipCode << " "
			               << current->realtyCompany << endl;
				
				current = current->link; 
				
			}
			
			outputFile.close();		
		}
			    
	}
	else
		do
		{
			cout << "All changes will be lost. Proceed (Y/N)?: "; 
			cin >> confirm; 
			cout << endl << endl; 
	
			confirm = toupper(confirm); 
	
			if (confirm != YES && confirm != NO) 
			cout << "Invalid Input - Must be 'Y' or 'N'" << endl << endl;
		}
		while (confirm != YES && confirm != NO);
	
	}
	while (confirm != YES && saveOption != YES); 	
	
}


//*****************************************************************************
// FUNCTION: ChangeAskingPrices
// DESCRIPTION: Allows user to apply price changes from file.    
// INPUT: Parameters: first - Pointer variable for first node in linked list.  
// last - Pointer variable for last node in linked list. 
// OUTPUT: Output direct to file.   
//***************************************************************************** 
void ChangeAskingPrices(listingsInfo* first, listingsInfo* last)
{
	// Function local variables
	ifstream changesFile; 		 // To receive changes file 
	int matchesFound = 0;		 // Counter to track the number of matches found 
	bool found;					 // To track whether MLS number exists in list   			
	int mlsToSearch;			 // To store MLS to search during each loop pass
	double reduction; 			 // To store reduction amount 
 
	listingsInfo *searchNode;	 // To hold node to search during each loop pass 
	
	changesFile.open(FILE_CHANGES.c_str());
	
	if(!changesFile)
	   cout << "Changes file does not exist" << endl << endl;  
	else
	{ 
	
		if(first == NULL)
			cout << "There are no records currently on file to search." << endl << endl; 
		else
		{		
			changesFile >> mlsToSearch; 
			
			while(changesFile)
			{
			   found = false; 
	   
	   		   searchNode = first;	
				
			   changesFile >> reduction; 
			
			   while(searchNode != NULL && !found)
			   { 
	   
	   				if (searchNode->numberMLS == mlsToSearch)
	   				{
	   					found = true;
						   
						searchNode->price = searchNode->price - reduction;    
						    
	   					matchesFound++; 
						   
						if (matchesFound == 1)	
						{
							cout << right << "MLS number" << setw(21) << "New Asking Price" << endl;
							cout << "----------" << setw(21) << "----------------" << endl; 
						}
					
						cout << searchNode->numberMLS << setw(15) << searchNode->price << endl; 	
						
	   				}
	   				else 
	   				{
	   					searchNode = searchNode->link;	   
					}
				
			   }
			
			changesFile >> mlsToSearch;
			
			}
			
			if (matchesFound == 0)
				cout << "No matches were found for the file. No price reductions were made" << endl; 
			
		}
		
	cout << endl; 	
		
	}	
}





