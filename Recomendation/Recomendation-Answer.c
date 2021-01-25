#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct {
	char *name;
	int valueofbook[20];
	double similarity;
}User;

typedef struct {
	char *bookname;
	int bookid;
	double pred;
}Book;

void csvoku(char** booknames, User *users, FILE* file){
	int i=0, j = 0;
	char line[1024];

	// --------------------- ikinci satiri okumak --------------------
	fgets(line, 1024, file);			// birinci satiri okudum.
	char* tmp = strdup(line);			// kopyaladim ama gerek yoktu aslinda
	char * token = strtok(tmp, ";");	// token tanimladim
	token = strtok(NULL, ";");          // ilki User basligi idi onu sildim.
	
    while( token != NULL ) {
       booknames[i++] = token;
       token = strtok(NULL, ";");
    }
    booknames[i]=strtok(booknames[--i], "\n");
	// -----------------------------------------------------------------
	
	// ------------------- User'lari almak -----------------------------
	i=0;
	while(fgets(line, 1024, file) != NULL){	
		tmp = strdup(line);			// kopyaladim ama gerek yoktu aslinda
		token = strtok(tmp, ";");	// token tanimladim
		users[i].name = token;
		token = strtok(NULL, ";");
		
		j=0;
		while( token != NULL ) {
       		users[i].valueofbook[j++] = atoi(token);
       		token = strtok(NULL, ";");
    	}
    	i++;
	}
}

// ra ve rb ortak okudugu kitaplara gore hesaplanmistir.
double ortalama(User x, int *kitaplar, int oks){		// ortak okunan kitaplar ve sayisi ve kullanici geldi.
	
	int i=0,j=0;
	double okunanlarOrt, top=0;
	
	for(i=0; i<oks; i++){								// ortak kitaplar icin verilen puanlar toplaniyor 
		top += x.valueofbook[kitaplar[i]];
		j++;
	}		
	okunanlarOrt = top/j;								// Toplam ortak kitap sayisina=j bolunuyor.
	
	return okunanlarOrt;
}

// ra ve rb just okudugu kitaplara gore hesaplanmistir. (2.)
double ortalama2(User x, int BOOKNUMBER){
	int i=0,j=0;
	double okunanlarOrt, top=0;
	
	for(i=0; i<BOOKNUMBER; i++){
		if( x.valueofbook[i] != 0){
			top = top + x.valueofbook[i];
			j++;
		}
	}		
	okunanlarOrt = top/j;
	
	return okunanlarOrt;
}

double pearson(User a, User b, int BOOKNUMBER){			// sim(a,b) hesaplaniyor
		
	int i=0, oks=0;					   					// oks ortak kitap sayisi
	double ra=0, rb=0, usttop=0, alttop=0;
	double alttopa=0, alttopb=0, sim=0;
	int *kitaplar = (int*)malloc(sizeof(int));			// Ortak kitaplarin indislerinin tutuldugu dizi (0-7 arasi)
	
	for(i=0; i<BOOKNUMBER; i++){						// Ortak kitaplar ve sayisi tespit ediliyor.
		if( (a.valueofbook[i] != 0)  && (b.valueofbook[i] !=0)){
			kitaplar[oks++]= i;    						// kitabin adini tutuyor 0-7 arasi. (7. kitap Harry Pother mesela)
		}
	}
	
	if(oks==0){											// oks = 0'sa ortak kitap yok demektir. 
		printf(" %d oks 0", oks);
	}
	
	ra = ortalama(a, kitaplar, oks);					// ortalama ortak kitaplara gore hesaplanirsa
	rb = ortalama(b, kitaplar, oks);	
//	ra = ortalama2(a, BOOKNUMBER);						// ortalama kisinin okudugu kitaplar uzerine hesaplanirsa
//	rb = ortalama2(b, BOOKNUMBER);	
	
	for(i=0; i<oks; i++){								// Ana sim formulu uygulaniyor.
		usttop  += (a.valueofbook[kitaplar[i]]-ra)*(b.valueofbook[kitaplar[i]] - rb);
		alttopa += pow((a.valueofbook[kitaplar[i]]-ra), 2.0);
		alttopb += pow((b.valueofbook[kitaplar[i]]-rb), 2.0);
	}
	
	alttop = sqrt(alttopa)*sqrt(alttopb);
	sim = usttop/alttop;
	
	return sim;											// bulunan sim degeri geri donduruluyor.
}

int *ksim(int userindis, User *users, int k, int TRAIN, int BOOKNUMBER){// userindis = Alinan kullanici (NU1) - users: tüm kullanicilar - k: istenen benzer kullanici sayisi
	
	int i, j, location=0;								// location: en benzerler bulunurken kullanilacak degisken.
	double similarity[TRAIN];							// en benzer k taneye ait similarity degerlerini tutan dizi. (TRAIN:20 egitim data seti sayisi)
	int *ks=(int*)malloc(k*sizeof(int));				// ks k adet kullanicinin isimlerine ait addresslerin tutulacagi dizi. Geri dondurulecek.
	
	for(i=0; i<TRAIN; i++){								// alinan kullanici ile tüm digerlerinin similaritysi hesaplanýp ilgili kullanicilarin user.similarity sine isleniyor.
		similarity[i] =  pearson(users[userindis], users[i], BOOKNUMBER);
		users[i].similarity = similarity[i];
	}
	
	for(j=0; j<k; j++){									// En benzer k tanesi bulunuyor (en yüksek k tanesi.)
		for (i=0; i<TRAIN; i++){
	    	if (similarity[location] < similarity[i]){
	      		location = i;
	    	}
	  	}
	  	ks[j]=location;
		similarity[location] = -1;
	}
	
	return ks;										    // En benzer k tanesinin addreslerini tutan diizi geri donduruluyor.
}

int pred(int userindis, User *users, int k, int *ks, char ** booknames, int BOOKNUMBER){
	
	int i, j=0, unreadsize;
	double usttop=0, alttop=0;
	Book *books = (Book*) malloc(k * sizeof(Book));		// okunmayan kitaplar ve bunlarin predleri ve adlarini tutacagimiz struct
	int *unread = (int*)malloc(k*sizeof(int));			// okunmayan kitaplarin isimlerini tutacagimiz dizi.

	for(i=0; i<BOOKNUMBER; i++){						// find userindisin okumadigi kitaplar. bunlari diziye at.
		if(users[userindis].valueofbook[i] == 0){
			unread[j]=i;
			books[j].bookid=i;
			j++;
		}
	}
	unreadsize=j;										// userindisin okumadigi kitap sayisi =unreadsize
	if(unreadsize == 0){
		printf(" Kullanicinin okumadigi kitap yok!");
	}
	
	for(j=0; j<unreadsize; j++){
		usttop=0;
		alttop=0;
		for(i=0; i<k; i++){
			usttop += (users[ks[i]].similarity * (users[ks[i]].valueofbook[unread[j]] - ortalama2(users[ks[i]],BOOKNUMBER )));
			alttop += users[ks[i]].similarity;
		}
		books[j].pred = ortalama2(users[userindis], BOOKNUMBER) + (usttop/alttop);
	}
	
	printf("\n %s kullanicisinda okunmadigi kitaplar için tahmini begenme degerleri:\n", users[userindis].name);
	int max=0;
	for(i=0; i<unreadsize; i++){
		printf("\n   %d. '%s' -> %.3f ", i, booknames[books[i].bookid], books[i].pred);
		if(books[max].pred < books[i].pred){
			max = i;
		}
	}
	
	printf("\n\n Onerilen kitap : %s\n\n", booknames[books[max].bookid]);
}

int main(){
	
    FILE* file = fopen("RecomendationDataSet.csv", "r");
    int i=0, j=0, k=0, userindis=0;					// userindis alinan kullanici isminin yerini tutuyor. (NU1'in yeri 20. address gibi)
    char username[30], line[1024];
    int *ks;										// k adet similarity deðerinin tutuyor. 
	int USERSIZE, TRAINSIZE, BOOKNUMBER;			// Bu bilgiler dosyanin en basinda yaiziyor.
	
	fgets(line, 1024, file);						// ilk satirdaki genel bilgiler aliniyor.
	USERSIZE   = atoi(strtok(line, ";"));			// token tanimladim
	TRAINSIZE  = atoi(strtok(NULL, ";"));			// token tanimladim
	BOOKNUMBER = atoi(strtok(NULL, ";"));			// token tanimladim	
	printf("USERSIZE: %d, TRAINSIZE: %d, BOOKNUMBER:%d\n", USERSIZE, TRAINSIZE, BOOKNUMBER);

  	char** booknames = (char**) calloc(BOOKNUMBER, sizeof(char*));		//Kitap isimlerini tutuyor.
	User *users = (User*) malloc(USERSIZE * sizeof(User));				// Kullanici struct dizisi

  	// diger satirlar okunuyor.
    csvoku(booknames, users, file);								// CSV Okundu yerlestirildi...  ilk satýr bilgisini dönsün bu USERSIZE;TESTSIZE;BOOKNUMBER

	// --------------------- Take username and k value -----------------------------
	printf(" => Username giriniz: ");
	scanf("%s", &username);
	
	for(i=0;i<USERSIZE; i++){									// Alinan ismi buldu.
		if(strcmp(username, users[i].name) == 0){
			userindis=i;}}
	
	printf(" => k degerini giriniz: ");
	scanf("%d", &k);
	// ----------------------------------------------------------------------------
	
	ks = ksim(userindis, users, k, TRAINSIZE, BOOKNUMBER);								// k adet benzer kullanici bulundu ve bu kullanicilarin isimleri(addreesleri) ks'ye alindi.
	printf("\n --------------------------------------------------------------------------------------------");
	printf("\n %s kullanicisina en yakin kullanicilar(k=%d) ve hesaplanan pearson benzerlikleri sirasiyla, \n", users[userindis].name, k);
	for(i=0; i<k; i++){
//		printf(" %d. sim(%s, %s): %.3f \n",i, users[userindis].name, users[ks[i]].name, users[ks[i]].similarity);
		printf("  %d.  '%s': %.3f \n", i, users[ks[i]].name, users[ks[i]].similarity);
	}
	
	pred(userindis, users, k, ks, booknames, BOOKNUMBER);
	
	return 0;
}



