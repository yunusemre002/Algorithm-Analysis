#include<stdio.h>
#include<string.h>
#include<stdbool.h>

char *renkBul(int r){   						// Kullanicidan alinan sayilarin renk karsiliklarini donen fonksiyon.

	if (r == 1)
		return "kirmizi";
	else if (r == 2)
		return "mavi";
	else if (r == 3)
		return "yesil";
	else if (r == 4)
		return "mor";
	else if (r == 5)
		return "turuncu";
	else if (r == 6)
		return "sari";
	else if (r == 7)
		return "siyah";
	else if (r == 8)
		return "beyaz";
	else
		return NULL;
}


void convertMatrix(int N, int a[N][N]){		// Sayilardan olusan Matrix, renkler formatina donusturulur. Ekrana renk matrisini yazdirmak icin kullanildi.
	
	printf("\n-------------------------------\n");
	char renk[10];
	int i=0, j=0;

	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			strcpy(renk,renkBul(a[i][j]));
			printf("%s  ", renk);
			renk[0] = '\0';
		}
		printf("\n");
    }
	printf("-------------------------------\n");
}


void matrixGoster(int N, int a[N][N]){	// Sadece sayilar matrixini gostemek icin kullanilan fonksiyon

	int i=0, j=0;
    printf("\nMatris: \n");
    for(i=0; i<N; ++i)
        for(j=0; j<N; ++j){
            printf("%d  ", a[i][j]);
            if (j == N-1)
                printf("\n");
        }
	
}


int **cevir(int N, int a[N][N], int satirNo){  // Kendisine verilen matrix ve satir bilgisini alir ilgili satiri 1 kez saga shift eder. Ve Matrixi geri dondurur.
	
	int i,j;
	int b[N];
	
	convertMatrix(N, a);
	
	b[0]= a[satirNo][N-1];
	for(i=1; i<=N; i++){
		b[i]= a[satirNo][i-1];	}

	for(i=0; i<N; i++){
		a[satirNo][i]=b[i];	}
	
	return a;
}


bool isSafe(int N, int a[N][N], int row){	// ilgili satir icin ilgili yere uygun mu diye bakar. Uygunsa True degilse False doner.
 
    int i, j; 
    // Son satirin tüm elemanlarini tek tek üstteki satirlara ait ilgili columns'da aynisi var mi diye bakar.
	// Yani son satirla 0. satir elemanlarini son satirla 1. satir elemanlarini .... check et. eger ayni olan varsa "false" don */
	for (i = 0; i<row; i++)
    	for(j=0; j<N; j++){					// Sonuna kadar kontrol edildi.
        	if (a[row][j] == a[i][j]){
            	return false; }
		}  
	printf("\nrow = %d", row);
    return true; 
} 


bool solveNQUtil(int N, int board[N][N], int row) { 
	
	int i;
    
    if (row >= N) 		// base case: If all rows are placed then return true.
        return true; 
  
    for (i=0; i<N-1; i++) { 
        if (isSafe(N, board, row)) { 		// ilk satiri kontrol et bir üstteki ile uyum icinde mi evetse(renklerde cakisma yok ise) devam et yani:(hic bisi yapma ilgili satir icin)
            printf("\n%d. row is safe",row);	// Ekrana igili satirin güvenli old. bilgisi yazdiriliyor.
            if (solveNQUtil(N, board, row + 1)) 	// Satiri 1 arttir KENDINI CAGIR.(RECURSIVE) Bir sonraki satir icin kontrol edilir. Boyle boyle son satira kadar gidilirse True doner.
                return true; 
  
            // BACKTRACK : Eger Bir sonraki satir tüm shift edilmelere ragmen uygun bir hal almiyorsa bir gerideki satir sikintilidir. don geri ve onu cevir. Ve Artýk bir gerisi icin bak uygun mu 
			//uygunsa bir sonraki satira gecer. Eger uygun degilse asagisaki ilk if'in else'sine duser.
			printf("\nBACKTRACKING\n  %d. row is not safe Turn it (%d. step)...", row, i+1);
			cevir(N, board, row);
			printf("\n%d. row turned:", row);
			convertMatrix(N, board);
			  
        }
		else{	// ilk if'te uygun olmadigi anlasilirsa ilgili satir bir tur saga shift edilir. Taki uygun olana kadar. (Ve tabiki ayný konuma gelene kadar.)
			printf("\n%d. row is not safe Turn it (%d. step)...", row, i+1);
			cevir(N, board, row);
			printf("\n%d. row turned:", row);
			convertMatrix(N, board);
		} 
    } 
  	printf("\nBir adim geri...");
    // Eger matrisin ilk satiri(0. satir) N-1 kere dondürülmesine(tüm kombinasyonlar denenmesine) ragmen bir sonuc bulunamadiysa demekki olmuyor. "False" doner.
    return false; 
} 


int main(){
	
	int  i=0, j=0;			// N: Matris boyutu, i: Dongu degiskeni
	
	printf("kirmizi:1	mavi:2	 yesil:3   mor:4" 
		  "\nturuncu:5	sari:6	 siyah:7   beyaz:8" 
		  "\nYukarida ki renkler ve kodlari verilmistir."
		  "Bu kodlar dogrultusunda n*n lik matris doldurmaniz beklenmektedir.");

//	------------------------------Matrixi kullanicidan almak icin -------------------
	int N;
	printf("\nMatris boyutunu giriniz N : ");
	scanf("%d",&N);
//	int a[N][N];	
//	// Matrisin elemanlarini sakliyoruz
//    printf("\nMatris elemanlarini girin:\n");
//    for(i=0; i<N; ++i)
//        for(j=0; j<N; ++j)
//        {
//            printf("Eleman girin a[%d][%d]: ",i+1, j+1);
//            scanf("%d", &a[i][j]);
//        }
//	--------------------------------------------------------------------------------
	
//	int N=5;
	
//	// cozumsuz
//	int a[3][3]={ 3, 2, 4,
//			 	  2, 3, 4,
//				  4, 3, 2};

//	// Siralaniyor
//	int a[3][3]={ 3, 2, 4,
//			 	  4, 3, 2,
//				  4, 3, 2};

//	// Siralaniyor.
//	int a[4][4]={ 1, 2, 3, 4,
//			 	  1, 2, 3, 4,
//				  2, 3, 4, 1,
//				  3, 4, 1, 2};

//	// Cozumsuz
//	int a[4][4]={ 3, 2, 1, 4,
//			 	  1, 2, 3, 4,
//				  1, 4, 3, 2,
//				  3, 2, 4, 1};
	
//	// Cozumsuz
//	int a[6][6]={ 3, 2, 1, 4, 5, 6,
//			 	  1, 2, 3, 4, 6, 5,
//				  1, 4, 3, 6, 2, 5,
//				  3, 2, 6, 1, 4, 5,
//				  5, 1, 6, 2, 4, 3,
//				  3, 2, 1, 4, 5, 6};

//	// Siralaniyor.
//	int a[6][6]={ 3, 2, 1, 4, 5, 6,
//			 	  5, 6, 3, 2, 1, 4,
//				  1, 4, 5, 6, 3, 2,
//				  6, 3, 2, 1, 4, 5,
//				  2, 1, 4, 5, 6, 3,
//				  3, 2, 1, 4, 5, 6};

//	// Siralaniyor.
//	int a[5][5]={ 3, 7, 1, 8, 5,
//			 	  5, 3, 7, 1, 8,
//				  1, 8, 5, 3, 7,
//				  3, 7, 1, 8, 5,
//				  7, 1, 8, 5, 3 };

	// Cozumsuz.
	int a[5][5]={ 3, 7, 1, 8, 5,
			 	  5, 3, 7, 1, 8,
				  1, 8, 3, 5, 7,
				  3, 7, 1, 8, 5,
				  7, 1, 8, 5, 3 };

//	// Siralaniyor
//	int a[8][8]={ 1, 2, 3, 4, 5, 6, 7, 8,
//			 	  1, 2, 3, 4, 5, 6, 7, 8,
//				  1, 2, 3, 4, 5, 6, 7, 8,
//				  1, 2, 3, 4, 5, 6, 7, 8,
//				  1, 2, 3, 4, 5, 6, 7, 8,
//			 	  1, 2, 3, 4, 5, 6, 7, 8,
//				  1, 2, 3, 4, 5, 6, 7, 8,
//				  1, 2, 3, 4, 5, 6, 7, 8};

	matrixGoster(N, a);
	convertMatrix(N, a);	
	
	if (solveNQUtil(N, a, 0) == false){ 
		printf("\nSolution does not exist"); 
		return false; 
	}
    else{
    	printf("\n\nSiralanmis Matrix asagidaki gibidir:");
    	convertMatrix(N, a);
	}
		
	return 0;
}
