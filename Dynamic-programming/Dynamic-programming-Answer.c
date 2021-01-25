#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <ctype.h>
#define M 997
#define MM 996
#define N 30

typedef struct{   		// 1. sozluk icin
	char word[N];
	int key; 
}hashStruct;

typedef struct{ 		// 2. sozluk icin
	char word[N];
	char trueWord[N];
	int key; 
}hsTrue;

int createDictLine(char *fileName){      // verilen sozlukteki kelimelerin altalta yazildigi bir dosya olusturuldu. 

	FILE * fp = fopen(fileName, "r"); 
	
	FILE *yaz;
    yaz = fopen("smallDictionaryLine.txt", "w");
    
	if (fp == NULL) return 1; 
	char c; 
	int count = 0; 
	while((c = fgetc(fp)) != EOF) 
	{ 
		if(c == ' ' || c == '\n'){ 
			printf("\n"); 
			fprintf (yaz,"\n");
			++count; 
		} 
		else{ 
			printf("%c", c); 
			fprintf (yaz, "%c", c);
		} 
	} 
	fclose(fp); 
	fclose(yaz);

	printf("This file has %d words in it.", count);
	return count; 
}

int h1(int key){					//  hash 1
	return key % M;
}

int h2(int key){					// hash 2
	return 1 + (key % MM);
}

int hash(int key, int i) {			// Main hash
 return (h1(key) + i*h2(key)) % M;
}

int horner(char *dizi){				// kelimeyi sayiya ceviriyor
	int sum=0; int i; int hornerPrime=3;
	for(i=0; i<strlen(dizi); i++){
		sum += pow(hornerPrime,i) * (int)dizi[i];
		//printf("%d, %c, %d sum=%d\n", i, dizi[i], (int)dizi[i], sum);
	}
	return sum;
}

// -1=deleted,  0=empty,  1=occupied  (1. sozlukte ki key elemninin degerleri )
void insertHashTable(hashStruct *ht, char *words){    	  // 1. sozlugugu hash table'a yerlestiriyor.
	
	int k;
	char wordLow[N];
	strcpy(wordLow,words);
	for(k = 0; k<=sizeof(wordLow); k++){
  		wordLow[k] = tolower(wordLow[k]);
	}

	int rotation = 0;
	
	int hash_value = hash(horner(wordLow), rotation++);
	int i = hash_value;

	while (ht[i].key != 0) {
		if (!strcmp(ht[i].word, wordLow)) {   			// zaten bu kelime var!
			fprintf(stderr,"Duplicate entry\n %s = %s",ht[i].word, wordLow);
			exit(1);
		}
		i = hash(horner(wordLow), rotation++);			// kelime yoksa yeri dolu demek ki yeni hash üret. Ve while'a git tekrar
		if (rotation == M) {							// rotation = Table hash table size ise tablo dolmus demektir.		
			fprintf(stderr, "The table is full\n");
			exit(1);
		}
	}
	strcpy(ht[i].word, wordLow);
	ht[i].key= 1;
	
}

int calculateDistance(int len1, int len2,const char *word1, char *word2){  // sadece verilen 2 kelimenin distancesi ni hesaplar. 
	
	int i;
    int matrix[len1 + 1][len2 + 1];
   
    for (i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (i = 0; i <= len2; i++) {
        matrix[0][i] = i;
    }
    
    int flag=0;					// bonus kismi flag ile yapildi. Flag, ilgili satirdaki 2 den buyuk eleman sayisini tutuyor.
    
    for (i = 1; i <= len1; i++) {
        int j;
        char c1;
		
		//[bonus: Eger flag degeri satir eleman sayisina ulasmis ise o satirin tamami 2 den buyuk demektir.
		if(flag==len2){
			return M*10;
		}
		
		flag=0;
		//bonus]
		
        c1 = word1[i-1];
        for (j = 1; j <= len2; j++) {
            char c2;

            c2 = word2[j-1];
            if (c1 == c2) {
                matrix[i][j] = matrix[i-1][j-1];
            }
            else {
                int delete;
                int insert;
                int substitute;
                int minimum;

                delete = matrix[i-1][j] + 1;
                insert = matrix[i][j-1] + 1;
                substitute = matrix[i-1][j-1] + 1;
                minimum = delete;
                if (insert < minimum) {
                    minimum = insert;
                }
                if (substitute < minimum) {
                    minimum = substitute;
                }
                matrix[i][j] = minimum;
            }
            if(matrix[i][j]>2){
        		flag++;
        		//printf("\n%d", flag);
			}
        }
    }
	return matrix[len1][len2];		// 3 ten kucuk olanlar icin doner. (istege gore 1. Bir yok ise 2 doner)
	
}

 // 1. hasteki kelimeleri tek tek alir ve verilen kelime ile olan uzakligina bakar
int LevenshteinEditDistance( hashStruct *ht, const char *word1, char *oneriListesi[], int icinBakilacak){ 

	int len1 = strlen(word1);
	int i, oneriIndex = 0, indis=0;
	
	for(indis=0; indis<M; indis++)
	{											          
		if(ht[indis].key==1){									// Birinci sozlukteki dolu olanlara bakacagiz.
			char *word2 =  ht[indis].word;
			int len2 = strlen(word2);
			//printf("\n%s ile %s arastiriliyor...", word2, word1);
			
			int gelen = calculateDistance(len1,len2,word1,word2);
			if(gelen < 3){
				if(gelen < icinBakilacak){
			    	printf("\n%s --distance: %d", word2, gelen);
			    	oneriListesi[oneriIndex] = word2;
			    	oneriIndex++;
				}
			}
		}
	}
	oneriListesi[oneriIndex] = NULL;							// sonucta bir dizi doner bu dizi benzer kelimeler dizisidir.
	return 0;
}

char *searchHashTableTrue( hashStruct *ht, hsTrue *htt, char *pch){   // Yanlis kelime geldi htt de aratilacak
	
	int rotation = 0;
	char *oneriListesi[N/2], *anacumle;
	int j = 0;
	
	int hash_value = hash(horner(pch), rotation++); 
	int i = hash_value;
	
	while (htt[i].key != 0 && strcmp(htt[i].word, pch)!=0 && rotation<M) {   // ilgili konum Occupied ise && kelimeler ayni degilse && rotationun sonuna gelmediyse devam et:
		i = hash(horner(pch), rotation++);			// Sonraki (yeni) hash'i üret.
	}
	
	if( htt[i].key == 0){
		printf("HashTable 2'de de yok! Benzer kelimeler arastiriliyor...");
		// kelime hatalilar sözlügünde de YOKSA! BASLA... Levenshtein Edit Distance :
		
		int icinBakilacak = 2;
		LevenshteinEditDistance(ht, pch, oneriListesi, icinBakilacak);
		if(oneriListesi[0] == NULL){
			// DiSTANCE 1 OLAN YOKTUR.  distancesi: 2 olan var mi diye baksin!
			printf("\nDistance:1 olan kelime yok distance:2 olana var mi bakiliyor...");
			icinBakilacak = 3;
			LevenshteinEditDistance(ht, pch, oneriListesi, icinBakilacak);
		}

		// NULL degilse ya dist:1 yada dist:2 ile dolmus demektir.
		if (oneriListesi[0] != NULL) 
		{
			char trueWordOfUser[N];
			j=0;
			printf("\n\n'%s' is't in the dict. Did you mean : ", pch);
			while(oneriListesi[j] != NULL){
				printf(" '%s' or  ", oneriListesi[j]);
				j++;
			}
			
			printf("?     :");
	   		scanf("%s",&trueWordOfUser);
	   		
	   		strcpy(htt[i].word, pch);
	   		strcpy(htt[i].trueWord,trueWordOfUser);
	   		htt[i].key = 1;
	   		return htt[i].trueWord;
		}
		else{
			printf("\nDistance'si <=2 olan bir kelime bulunamadi.'\n");
			return NULL;
		}
	}	
	else if( strcmp(htt[i].word, pch)==0 ) {   // VARSA -- Kelime bulundu. return dogru olani
		return htt[i].trueWord;
	}

}

void searchAtHashTable(hashStruct *ht, char cumle[], hsTrue *htt){	
	
	int rotation = 0;
	//printf("%s",cumle);
	
	// Cumlenin kelimelerini cikart   
	char *arr[N/2];
	int indexOfArr=0;
	
	
	char *pch;	
  	pch = strtok (cumle," ,.-\n");
	  
  	while (pch != NULL)		// Her bir kelime için search islemi yapiliyor.
  	{
    	//printf ("%s\n",pch);
		rotation = 0;
		
    	int hash_value = hash(horner(pch), rotation++);
		int i = hash_value;
		
		while (ht[i].key != 0 && strcmp(ht[i].word, pch)!=0 && rotation<M) {   // ilgili konum Occupied ise && kelimeler ayni degilse && rotationun sonuna gelmediyse devam et:
			i = hash(horner(pch), rotation++);			// Sonraki (yeni) hash'i üret.
		}
		
		if( ht[i].key == 0){
			// kelime sözlükte yok! BASLA... Hatali sozluk tablosunda aramaya :
			printf("\n'%s' 1. sozlukte yok!  Hash Table 2'ye bakiliyor...\n", pch);
			char *newWord = searchHashTableTrue(ht, htt, pch);
			if (newWord == NULL){	// GELEN SOZCUK NULL ISE
				// pch degismiyecek
				arr[indexOfArr] = pch;
			}
			else{
			//	strcpy(pch,newWord);
				arr[indexOfArr] = newWord;
			}
			
			
		}
		else if( strcmp(ht[i].word, pch)==0 ) {   // zaten bu kelime var. Tamam sikinti yok, bisi yapma!
			printf("'%s' sozlukte var\n", ht[i].word, pch);
			// Bunlarý sirasiyla dizide tutmak lazim sonra ya ekrana yazdiririz yada dondururuz geri.
			arr[indexOfArr] = pch;
		}
	    
	    pch = strtok(NULL," ,.-\n");
	    indexOfArr++;
  	}

	arr[indexOfArr] = NULL;
	
 	printf ("\n\nNew Sentence  :");
 	indexOfArr=0;
  	while (arr[indexOfArr] != NULL)		// Her bir kelime için search islemi yapiliyor.
  	{
    	printf (" %s ",arr[indexOfArr]);
    	indexOfArr++;
	}
}


void hashMap_yazdir( hashStruct* ht){
	int i=0;
	for(i = 0; i < M; i++){
		if(ht[i].key>=0){
			printf("adres:%d  - %s\n",ht[i].word, ht[i].key);
		}
	}
}
	
void kullanici_islemleri(hashStruct *ht, hsTrue *htt){

	int i;
	
	// Kullanicidan cumle al.
	char cumle[10*N];
	printf("\n\n-------------------------------------------------------------");
	printf("\nBir cumle giriniz : "); 
   	fgets(cumle, 10*N, stdin);   // klavyeden degil stdin'den okuyormusuzda bilmem ne! iste bunlar  OS'a aitlermis

   	// cumle lowercase
   	for(i = 0; i<=sizeof(cumle); i++){
  		cumle[i] = tolower(cumle[i]);
	}
	//printf("%s", cumle);

  	searchAtHashTable(ht, cumle, htt);
   
}

int main(void) 
{ 
	//int count = createDictLine("smallDictionary.txt");   // Just convert dict. From mow on, Each line have one word
	hashStruct *ht = (hashStruct*) calloc(M, sizeof(hashStruct));
	hsTrue *htt = (hsTrue*) calloc(M, sizeof(hsTrue));
	
	// Fill HashTable state with 0 (0 is Emty, -1 is deleted, 1 is occupied)
	int i;
	for(i=0; i<M; i++){   // 0 is Empty
		ht[i].key = 0;
		htt[i].key = 0;
		strcpy(ht[i].word,"");
	}

	
	// 	Create Fist hash table
	char line[256];
	FILE *file1 = fopen("smallDictionaryLine.txt", "r");
    while (fgets(line, sizeof(line), file1)) {
    	line[strcspn(line, "\n")] = 0;                  // string sonundaki \n'i kaldirir.
		//printf("%s : %d\n", line, horner(line)); 
        insertHashTable(ht, line);
    }
    
    
    // Write HashTable to file
    FILE *file2 = fopen("ht.txt", "w");
    for(i=0; i<M; i++){
		//printf("%s - %d \n", ht[i].word, ht[i].key); 
		fprintf (file2, "%s\n", ht[i].word);
	}
	
	fclose(file1);
	fclose(file2);
	
	int devamInt=1;
	char devam;
	while(devamInt){
		kullanici_islemleri(ht, htt);
		/*
		printf("\nDevam etmek istiyor musunuz {e/h} :");
		scanf("%c",&devam);
		
		if(devam == 'e')
			devamInt = 1;
		else
			break;*/
	}
	return 0; 
	    
} 
