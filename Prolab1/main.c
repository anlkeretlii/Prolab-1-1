#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <math.h>
#undef DrawText

#define SATIR 26
#define SUTUN 20

#define URL_YAPBENZET "https://yapbenzet.org.tr/"
#define URL_KOCAELI "https://bilgisayar.kocaeli.edu.tr/"


#define MAX_HERO_NAME_LENGTH 50
#define MAX_UNIT_NAME_LENGTH 15
#define MAX_HEROES 5
#define MAX_ORCS 4

#define MAX_HUMAN_CREATURE 5
#define MAX_ORK_CREATURE 6
#define MAX_CREATURE_NAME_LENGTH 25
#define ARRAY_SIZE 1024


const int ekranEni=1000;
const int ekranBoy=1300;
const int hucreEni=ekranEni/SUTUN;
const int hucreBoy=ekranBoy/SATIR;

const int array_size=4000;

char insanBirimAd[5][20]={"piyadeler","okcular","suvariler","kusatma_makineleri","tum_birimler"};
char orkBirimAd[5][20]={"ork_dovusculeri","mizrakcilar","varg_binicileri","troller","tum_birimler"};
char birimAdlari[9][20]={"piyadeler","okcular","suvariler","kusatma_makineleri","ork_dovusculeri","mizrakcilar","varg_binicileri","troller","tum_birimler"};
char birimAdlari_Yaz[8][25]={"Piyadeler'in","Okcular'in","Suvariler'in","Kusatma_makineleri'nin","Ork_dovusculeri'nin","Mizrakcilar'in","Varg_binicileri'nin","Troller'in"};
char birimAnahtarlari[4][20]={"saldiri","savunma","saglik","kritik_sans"};

char Seviyeler[3][10]={"seviye_1","seviye_2","seviye_3"};
char arastirmaAdlari[4][21]={"savunma_ustaligi","saldiri_gelistirmesi","elit_egitim","kusatma_ustaligi"};


char insanKahramanAdlari[MAX_HEROES][MAX_HERO_NAME_LENGTH] = {
    "Alparslan", "Fatih_Sultan_Mehmet", "Mete_Han", "Yavuz_Sultan_Selim", "Tugrul_Bey"
};
char orkKahrarmanAdlari[MAX_ORCS][MAX_HERO_NAME_LENGTH] = {
    "Goruk_Vahsi", "Thruk_Kemikkiran", "Vrog_Kafakiran", "Ugar_Zalim"
};
char kahmaranCanavarKeyleri[3][MAX_UNIT_NAME_LENGTH] = {
    "saldiri", "savunma", "kritik"
};

struct hero {
    int etki[3]; // 0=saldiri, 1=savunma, 2=kritik
    char etkiledigi_birim[MAX_UNIT_NAME_LENGTH];
    int etkiledigi_index;
    bool varmi;
};


struct hero insanKahramanlari[MAX_HEROES];
struct hero orkKahramanlari[MAX_ORCS];

void kahraman_deger_oku(char* ilk_pointer, struct hero* Hero);
int kahramanları_dosyadan_oku(const char* filename, char* buffer, size_t size);

struct birim
{
    int saldiri;
    int savunma;
    int saglik;
    int kritik_sans;
    int birim_sayisi;
    int ilk_birim_sayisi;
    int savunma_gucu;
    int saldiri_gucu;
    int toplam_saglik;
};

struct Cell
{
    int birim_sayisi;
    int ilk_birim_sayisi;
    int birim_index;
    bool hasUnit;
    bool isVisible;
};

struct creature
{
    int etki[3]; // 0=saldiri, 1=savunma, 2=kritik
    char etkiledigi_birim[15];
    int etkiledigi_index;
    bool varmi;
};

char insanYaratikAdlari[MAX_HUMAN_CREATURE][MAX_CREATURE_NAME_LENGTH] = {
    "Ejderha","Agri_Dagi_Devleri","Tepegoz","Karakurt","Samur"};

char orkYaratikAdlari[MAX_ORK_CREATURE][MAX_CREATURE_NAME_LENGTH] = {
    "Kara_Troll","Golge_Kurtlari","Camur_Devleri","Ates_Iblisi","Makrog_Savas_Beyi","Buz_Devleri"
};

struct creature insanYaratik[5]; // 0=Ejderha, 1=Agri_Dagi_Devleri, 2=Tepegoz, 3=Karakurt, 4=Samur
struct creature orkYaratiklar[6]; // 0=Kara_Troll, 1=Golge_Kurtlari, 2=Camur_Devleri, 3=Ates_Iblisi, 4=Makrog_Savas_Beyi, 5=Buz_Devleri

int parse_bonus_deger(const char* json_fragment);
void parse_etki_turu(const char* json_fragment, struct creature* Creature, int bonus_deger);
void yaratiklarin_degerlerini_bul(const char* json_fragment, struct creature* Creature);

struct research
{
    int etki;
    char etkiledigi_birim[20];
    int etkiledigi_index;
    bool varmi;
};

struct birim birimler[8]; // 0=piyadeler, 1=okcular, 2=suvariler, 3=kusatma_makineleri, 4=ork_dovusculeri, 5=mizrakcilar, 6=varg_binicileri, 7=troller

struct Cell ekran[SUTUN][SATIR];

struct research Insan_Arastirma[4][3]; // 0=savunma_ustaligi, 1=saldiri_gelistirmesi, 2=elit_egitim, 3=kusatma_ustaligi
struct research Ork_Arastirma[4][3];

int birimDegerleriOku(char* ilk_pointer, char* anahtar);

void senaryodan_deger_oku(char* ilk_pointer, struct birim birimler[], struct hero Heros[], struct creature Creatures[], struct research Arastirma[][3]);
int saldiri_gucu_hesapla(struct birim Unit, int birim_index, struct creature Creatures[], struct hero Heros[], struct research Arastirma[][3], int hero_sayisi, int creature_sayisi, int tur);
int savunma_gucu_hesapla(struct birim Unit, int birim_index, struct creature Creatures[], struct hero Heros[], struct research Arastirma[][3], int hero_sayisi, int creature_sayisi, int tur);
void saldiri_etkisi_hesapla(int dusman_saldiri_gucu, int toplam_savunma_gucu, struct birim* Unit);

void randomYerlestir(struct birim Unit, int birim_index, struct Cell ekran[][26]);
void draw_cell(struct Cell ekran, int i, int j);

Texture2D birimGorsel[8];
Texture2D orkKahramanGorsel[4];
Texture2D insanYaratikGorsel[5];
Texture2D orkYaratikGorsel[6];

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);


void guc_hesapla(struct birim *birimler, int start, int end,struct creature *yaratiklar,struct hero *kahramanlar,struct research *arastirma, int param1, int param2, int tur, int *toplam_saldiri_gucu, int *toplam_savunma_gucu) {
    for (int i = start; i < end; i++) {
        birimler[i].saldiri_gucu = saldiri_gucu_hesapla(birimler[i], i, yaratiklar, kahramanlar, arastirma, param1, param2, tur);
        *toplam_saldiri_gucu += birimler[i].saldiri_gucu;

        birimler[i].savunma_gucu = savunma_gucu_hesapla(birimler[i], i, yaratiklar, kahramanlar, arastirma, param1, param2, tur);
        *toplam_savunma_gucu += birimler[i].savunma_gucu;
    }
};


int main()
{
    char senaryo[35];

    FILE *birim_types=fopen("Files/unit_types.json","r");
    if (birim_types==NULL)
    {
        printf("Acilamayan dosya: unit_types.json file!\n");
        return 1;
    }
    char birim_types_string[array_size];
    int birim_types_json_length=fread(birim_types_string,sizeof(char),array_size-1,birim_types);
    birim_types_string[birim_types_json_length]='\0'; // Stringi sonlardirmak için sonuna '\0' koyuyoruz.
    fclose(birim_types);

    for (int i=0;i<4;i++)
    {
        birimler[i+4].saldiri=birimDegerleriOku(strstr(birim_types_string,orkBirimAd[i]),birimAnahtarlari[0]);
        birimler[i+4].savunma=birimDegerleriOku(strstr(birim_types_string,orkBirimAd[i]),birimAnahtarlari[1]);
        birimler[i+4].saglik=birimDegerleriOku(strstr(birim_types_string,orkBirimAd[i]),birimAnahtarlari[2]);
        birimler[i+4].kritik_sans=birimDegerleriOku(strstr(birim_types_string,orkBirimAd[i]),birimAnahtarlari[3]);
        birimler[i+4].birim_sayisi=0;

        birimler[i].saldiri=birimDegerleriOku(strstr(birim_types_string,insanBirimAd[i]),birimAnahtarlari[0]);
        birimler[i].savunma=birimDegerleriOku(strstr(birim_types_string,insanBirimAd[i]),birimAnahtarlari[1]);
        birimler[i].saglik=birimDegerleriOku(strstr(birim_types_string,insanBirimAd[i]),birimAnahtarlari[2]);
        birimler[i].kritik_sans=birimDegerleriOku(strstr(birim_types_string,insanBirimAd[i]),birimAnahtarlari[3]);
        birimler[i].birim_sayisi=0;
    }

    char heroes_string[ARRAY_SIZE];
    if (kahramanları_dosyadan_oku("Files/heroes.json", heroes_string, sizeof(heroes_string)) != 0) {
        return 1;
    }

    for (int i = 0; i < MAX_HEROES; i++) {
        kahraman_deger_oku(strstr(heroes_string, insanKahramanAdlari[i]), &insanKahramanlari[i]);
        insanKahramanlari[i].varmi = false;
    }

    for (int i = 0; i < MAX_ORCS; i++) {
        kahraman_deger_oku(strstr(heroes_string, orkKahrarmanAdlari[i]), &orkKahramanlari[i]);
        orkKahramanlari[i].varmi = false;
    }

    FILE* creatures = fopen("Files/creatures.json", "r");
    if (creatures == NULL) {
        printf("Acilamayan dosya: creatures.json file!\n");
        return 1;
    }

    char creatures_string[ARRAY_SIZE];
    int creatures_json_length = fread(creatures_string, sizeof(char), ARRAY_SIZE - 1, creatures);
    creatures_string[creatures_json_length] = '\0';
    fclose(creatures);

    struct creature insanYaratik[5];
    struct creature orkYaratiklar[6];

    // İnsan yaratıklarını okuma
    for (int i = 0; i < 5; i++) {
        const char* pos = strstr(creatures_string, insanYaratikAdlari[i]);
        if (pos) {
            yaratiklarin_degerlerini_bul(pos, &insanYaratik[i]);
            insanYaratik[i].varmi = true;
        } else {
            insanYaratik[i].varmi = false;
        }
    }

    // Ork yaratıklarını okuma
    for (int i = 0; i < 6; i++) {
        const char* pos = strstr(creatures_string, orkYaratikAdlari[i]);
        if (pos) {
            yaratiklarin_degerlerini_bul(pos, &orkYaratiklar[i]);
            orkYaratiklar[i].varmi = true;
        } else {
            orkYaratiklar[i].varmi = false;
        }
    }


    FILE *research=fopen("Files/research.json","r");
    if (research==NULL)
    {
        printf("Acilamayan dosya: research.json file!\n");
        return 1;
    }
    char research_string[array_size];
    int research_json_length=fread(research_string,sizeof(char),array_size-1,research);
    research_string[research_json_length]='\0';
    fclose(research);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
        arastirma_degerlerini_bul(research_string, Seviyeler[j], &Insan_Arastirma[i][j]);
        Insan_Arastirma[i][j].varmi = false;

        arastirma_degerlerini_bul(research_string, Seviyeler[j], &Ork_Arastirma[i][j]);
        Ork_Arastirma[i][j].varmi = false;
        }
    }

    CURL *curl_handle;
    CURLcode res;
    char scenario_string[2048] = {0};

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle=curl_easy_init();

    //curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYSTATUS, 0);
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, "Files\\cacert.pem");
    //curl_easy_setopt(curl_handle, CURLOPT_CAPATH, "Files\\cacert.pem");

    int num;
    printf("Similasyon icin senaryo seciniz: ");
    scanf("%d",&num);
    if (num<1 || num>10){
        printf("Gecersiz bir senaryo sectiniz. Tekrar deneyiniz.\n");
    }
    while(num<1 || num>10){
        scanf("%d",&num);
        if (num<1 || num>10)
        printf("Gecersiz bir senaryo sectiniz. Tekrar deneyiniz.\n");
    }

    char temp[50]; // Geçici bir dizi

    if(num<=5){
        strcpy(senaryo, URL_YAPBENZET); // URL'yi kopyala
        sprintf(temp, "%d.json", num); // JSON uzantısını oluştur
        strcat(senaryo, temp); // JSON uzantısını ekle
        printf("%s\n", senaryo); // Sonucu yazdırS
    }
    else
    {
        strcpy(senaryo, URL_KOCAELI);
        sprintf(temp, "%d.json", num-5); // JSON uzantısını oluştur
        strcat(senaryo, temp); // JSON uzantısını ekle
        printf("%s\n", senaryo); // Sonucu yazdırS
    }

    if(curl_handle) {
        // Ýstenilen URL
        curl_easy_setopt(curl_handle, CURLOPT_URL, senaryo);

        // Veriyi callback ile almak için ayar
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Veriyi kaydedeceðimiz buffer
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, scenario_string);

        // Ýsteði gönder
        res = curl_easy_perform(curl_handle);

        // Ýstek baþarýlý oldu mu?
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            //printf("%s", scenario_string);  // Gelen veriyi ekrana yazdýr
        }
        // Temizlik
        curl_easy_cleanup(curl_handle);
    }

    senaryodan_deger_oku(strstr(scenario_string,"ork_legi"),birimler,orkKahramanlari,orkYaratiklar,Ork_Arastirma);
    senaryodan_deger_oku(strstr(scenario_string,"insan_imparatorlugu"),birimler,insanKahramanlari,insanYaratik,Insan_Arastirma);

    int insan_saldiri_gucu=0;
    int insan_savunma_gucu=0;
    int ork_saldiri_gucu=0;
    int ork_savunma_gucu=0;
    int tur=1;
    int ork_flag=0;
    int insan_flag=0;

    for (int i=0;i<4;i++)
    {
        birimler[i].toplam_saglik=birimler[i].saglik*birimler[i].birim_sayisi;
        birimler[i+4].toplam_saglik=birimler[i+4].saglik*birimler[i+4].birim_sayisi;
    }

    FILE* Similasyon_Sonuc=fopen("Similasyon_Sonuc.txt","w");
    if (Similasyon_Sonuc==NULL)
    {
        printf("Error while opening Similasyon_Sonuc.txt file!\n");
        return 1;
    }
    //---------------------------Oyunun Basladigi Yer---------------------------//
    srand(time(0));
    InitWindow(ekranEni,ekranBoy,"Savas Görsel");

    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    SetWindowPosition((screenWidth - ekranEni) / 2, (screenHeight - ekranBoy) / 2);

    SetTargetFPS(25);

    birimGorsel[0]=LoadTexture("Icons\\piyadeler.png");
    birimGorsel[1]=LoadTexture("Icons\\okcular.png");
    birimGorsel[2]=LoadTexture("Icons\\suvariler.png");
    birimGorsel[3]=LoadTexture("Icons\\kusatma_makineleri.png");
    birimGorsel[4]=LoadTexture("Icons\\ork_dovusculeri.png");
    birimGorsel[5]=LoadTexture("Icons\\mizrakcilar.png");
    birimGorsel[6]=LoadTexture("Icons\\varg_binicileri.png");
    birimGorsel[7]=LoadTexture("Icons\\troller.png");

    for (int i=0;i<SUTUN;i++)
    {
        for (int j=0;j<SATIR;j++)
        {
            ekran[i][j].birim_sayisi=0;
            ekran[i][j].hasUnit=false;
            ekran[i][j].birim_index=-1;
            if (j>20)
            {
                ekran[i][j].isVisible=false;
            }
            else
            {
                ekran[i][j].isVisible=true;
            }
        }
    }

    for (int i=0;i<8;i++)
    {
       randomYerlestir(birimler[i],i,ekran);
    }

    // İnsan birimleri için hesaplama
    guc_hesapla(birimler, 0, 4, insanYaratik, insanKahramanlari, Insan_Arastirma, 5, 5, tur, &insan_saldiri_gucu, &insan_savunma_gucu);

    // Ork birimleri için hesaplama
    guc_hesapla(birimler, 4, 8, orkYaratiklar, orkKahramanlari, Ork_Arastirma, 4, 6, tur, &ork_saldiri_gucu, &ork_savunma_gucu);


    int time=0;
    int frames=0;

    while (!WindowShouldClose())
    {
        frames++;
        if (frames%10==0)
        {
            time++;
        }

        BeginDrawing();
        ClearBackground(LIGHTGRAY);


        for (int i=0;i<SUTUN;i++)
        {
            for (int j=0;j<SATIR;j++)
            {
                draw_cell(ekran[i][j], i, j);
            }
        }

        draw_toplam_gucler(insan_savunma_gucu,insan_saldiri_gucu,ork_savunma_gucu,ork_saldiri_gucu);

        if (ork_flag==1)
        {
            DrawText("#INSANLAR KAZANDI#",4*hucreEni+(12*hucreEni-MeasureText("#INSANLAR KAZANDI#",20))/2,24*hucreBoy+(hucreBoy-20)/2,20,RED);
        }
        else if (insan_flag==1)
        {
            DrawText("#ORKLAR KAZANDI#",4*hucreEni+(12*hucreEni-MeasureText("#ORKLAR KAZANDI#",20))/2,24*hucreBoy+(hucreBoy-20)/2,20,DARKGREEN);
        }

        if (time>=2)
        {

        time=0;

        if (tur%2!=0 && ork_flag!=1 && insan_flag!=1)
        {
            insan_saldiri_gucu=0;
            insan_savunma_gucu=0;
            ork_saldiri_gucu=0;
            ork_savunma_gucu=0;

            // İnsan birimleri için hesaplama
            guc_hesapla(birimler, 0, 4, insanYaratik, insanKahramanlari, Insan_Arastirma, 5, 5, tur, &insan_saldiri_gucu, &insan_savunma_gucu);

            // Ork birimleri için hesaplama
            guc_hesapla(birimler, 4, 8, orkYaratiklar, orkKahramanlari, Ork_Arastirma, 4, 6, tur, &ork_saldiri_gucu, &ork_savunma_gucu);

            for (int i=4;i<8;i++)
            {
                if (birimler[i].birim_sayisi!=0)
                {
                    saldiri_etkisi_hesapla(insan_saldiri_gucu,ork_savunma_gucu,&birimler[i]);
                }
            }
            insan_saldiri_gucu=0;
            insan_savunma_gucu=0;
            ork_saldiri_gucu=0;
            ork_savunma_gucu=0;
            if (birimler[4].birim_sayisi==0 && birimler[5].birim_sayisi==0 && birimler[6].birim_sayisi==0 && birimler[7].birim_sayisi==0)
            {
                ork_flag=1;
                ork_savunma_gucu=0;
                ork_saldiri_gucu=0;
            }

            // Ork birimleri için hesaplama
            guc_hesapla(birimler, 4, 8, orkYaratiklar, orkKahramanlari, Ork_Arastirma, 4, 6, tur, &ork_saldiri_gucu, &ork_savunma_gucu);
            // İnsan birimleri için hesaplama
            guc_hesapla(birimler, 0, 4, insanYaratik, insanKahramanlari, Insan_Arastirma, 5, 5, tur, &insan_saldiri_gucu, &insan_savunma_gucu);


            fprintf(Similasyon_Sonuc,
                "#################################\n"
                "%d. Tur Sonuclari:\n\n"
                "Insan Ordusu:\n"
                "Insanlarin saldiri gucu: %d\n"
                "Insanlarin savunma gucu: %d\n\n",
                tur, insan_saldiri_gucu, insan_savunma_gucu
            );


            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s saldiri gucu: %d\n",birimAdlari_Yaz[i],birimler[i].saldiri_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s savunma gucu: %d\n",birimAdlari_Yaz[i],birimler[i].savunma_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            fprintf(Similasyon_Sonuc,"Insanlarin birim sayilari:\n");
            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s birim sayisi: %d\n",birimAdlari_Yaz[i],birimler[i].birim_sayisi);
            }

            fprintf(Similasyon_Sonuc,
                    "\n\n\n"
                    "Ork Legi:\n"
                    "Orklarin saldiri gucu: %d\n"
                    "Orklarin savunma gucu: %d\n\n",
                    ork_saldiri_gucu, ork_savunma_gucu
            );

            for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s saldiri gucu: %d\n",birimAdlari_Yaz[i],birimler[i].saldiri_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s savunma gucu: %d\n",birimAdlari_Yaz[i],birimler[i].savunma_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            fprintf(Similasyon_Sonuc,"Orklarin birim sayilari:\n");
             for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s birim sayisi: %d\n",birimAdlari_Yaz[i],birimler[i].birim_sayisi);
            }
            fprintf(Similasyon_Sonuc,"\n\n\n");

            if (ork_flag==1)
            {
                fprintf(Similasyon_Sonuc,"\nInsanlar kazandi!\n");
            }

            if (ork_flag==1)
            {
                printf("Insanlar kazandi!\n");
                printf("Tur Sayisi: %d\n",tur-1);
                for (int i=0;i<SUTUN;i++)
                {
                    for (int j=0;j<SATIR;j++)
                    {
                        ekran[i][j].birim_sayisi=0;
                        ekran[i][j].hasUnit=false;
                        ekran[i][j].birim_index=-1;
                        if (j>20)
                        {
                            ekran[i][j].isVisible=false;
                        }
                        else
                        {
                            ekran[i][j].isVisible=true;
                        }
                    }
                }
                for (int i=0;i<8;i++)
                {
                    randomYerlestir(birimler[i],i,ekran);
                }
            }
            if (ork_flag!=1)
            {
                for (int i=0;i<SUTUN;i++)
                {
                    for (int j=0;j<SATIR;j++)
                    {
                        ekran[i][j].birim_sayisi=0;
                        ekran[i][j].hasUnit=false;
                        ekran[i][j].birim_index=-1;
                        if (j>20) //|| (j>=21 && i>-1 && i<50
                        {
                            ekran[i][j].isVisible=false;
                        }
                        else
                        {
                            ekran[i][j].isVisible=true;
                        }
                    }
                }
                for (int i=0;i<8;i++)
                {
                    randomYerlestir(birimler[i],i,ekran);
                }
                tur++;
                continue;
            }
        }

        if (tur%2==0 && ork_flag!=1 && insan_flag!=1)
        {
            insan_saldiri_gucu=0;
            insan_savunma_gucu=0;
            ork_saldiri_gucu=0;
            ork_savunma_gucu=0;

            // Ork birimleri için hesaplama
            guc_hesapla(birimler, 4, 8, orkYaratiklar, orkKahramanlari, Ork_Arastirma, 4, 6, tur, &ork_saldiri_gucu, &ork_savunma_gucu);
            // İnsan birimleri için hesaplama
            guc_hesapla(birimler, 0, 4, insanYaratik, insanKahramanlari, Insan_Arastirma, 5, 5, tur, &insan_saldiri_gucu, &insan_savunma_gucu);


            for (int i=0;i<4;i++)
            {
                if (birimler[i].birim_sayisi!=0)
                {
                    saldiri_etkisi_hesapla(ork_saldiri_gucu,insan_savunma_gucu,&birimler[i]);
                }
            }
            insan_saldiri_gucu=0;
            insan_savunma_gucu=0;
            ork_saldiri_gucu=0;
            ork_savunma_gucu=0;
            if (birimler[0].birim_sayisi==0 && birimler[1].birim_sayisi==0 && birimler[2].birim_sayisi==0 && birimler[3].birim_sayisi==0)
            {
                insan_flag=1;
                insan_savunma_gucu=0;
                insan_saldiri_gucu=0;
            }

            // İnsan birimleri için hesaplama
            guc_hesapla(birimler, 0, 4, insanYaratik, insanKahramanlari, Insan_Arastirma, 5, 5, tur, &insan_saldiri_gucu, &insan_savunma_gucu);
            // Ork birimleri için hesaplama
            guc_hesapla(birimler, 4, 8, orkYaratiklar, orkKahramanlari, Ork_Arastirma, 4, 6, tur, &ork_saldiri_gucu, &ork_savunma_gucu);



            fprintf(Similasyon_Sonuc,
                "#################################\n"
                "%d. Tur Sonuclari:\n\n"
                "Insan Ordusu:\n"
                "Insanlarin saldiri gucu: %d\n"
                "Insanlarin savunma gucu: %d\n\n",
                tur, insan_saldiri_gucu, insan_savunma_gucu
            );

            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s saldiri gucu: %d\n",birimAdlari_Yaz[i],birimler[i].saldiri_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s savunma gucu: %d\n",birimAdlari_Yaz[i],birimler[i].savunma_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            fprintf(Similasyon_Sonuc,"Insanlarin birim sayilari:\n");
            for (int i=0;i<4;i++)
            {
                fprintf(Similasyon_Sonuc,"%s birim sayisi: %d\n",birimAdlari_Yaz[i],birimler[i].birim_sayisi);
            }

            fprintf(Similasyon_Sonuc,
                    "\n\n\n"
                    "Ork Legi:\n"
                    "Orklarin saldiri gucu: %d\n"
                    "Orklarin savunma gucu: %d\n\n",
                    ork_saldiri_gucu, ork_savunma_gucu
            );

            for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s saldiri gucu: %d\n",birimAdlari_Yaz[i],birimler[i].saldiri_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s savunma gucu: %d\n",birimAdlari_Yaz[i],birimler[i].savunma_gucu);
            }
            fprintf(Similasyon_Sonuc,"\n");
            fprintf(Similasyon_Sonuc,"Orklarin birim sayilari:\n");
             for (int i=4;i<8;i++)
            {
                fprintf(Similasyon_Sonuc,"%s birim sayisi: %d\n",birimAdlari_Yaz[i],birimler[i].birim_sayisi);
            }
            fprintf(Similasyon_Sonuc,"\n\n\n");
            if (insan_flag==1)
            {
                fprintf(Similasyon_Sonuc,"\nOrklar kazandi!\n");
            }
            if (insan_flag==1)
            {
                printf("Orklar kazandi!\n");
                printf("Tur Sayisi: %d\n",tur-1);

                for (int i=0;i<SUTUN;i++)
                {
                    for (int j=0;j<SATIR;j++)
                    {
                        ekran[i][j].birim_sayisi=0;
                        ekran[i][j].hasUnit=false;
                        ekran[i][j].birim_index=-1;
                        if (j>20)
                        {
                            ekran[i][j].isVisible=false;
                        }
                        else
                        {
                            ekran[i][j].isVisible=true;
                        }
                    }
                }
                for (int i=0;i<8;i++)
                {
                    randomYerlestir(birimler[i],i,ekran);
                }
            }
            if (insan_flag!=1)
            {
                for (int i=0;i<SUTUN;i++)
                {
                    for (int j=0;j<SATIR;j++)
                    {
                        ekran[i][j].birim_sayisi=0;
                        ekran[i][j].hasUnit=false;
                        ekran[i][j].birim_index=-1;
                        if (j>20)
                        {
                            ekran[i][j].isVisible=false;
                        }
                        else
                        {
                            ekran[i][j].isVisible=true;
                        }
                    }
                }
                for (int i=0;i<8;i++)
                {
                    randomYerlestir(birimler[i],i,ekran);
                }
                tur++;
            }
        }

        }
        EndDrawing();
    }
    CloseWindow();
    fclose(Similasyon_Sonuc);
}


/* Kahraman dosyasını okuma ve degerleri ayrıstırma fonksiyonları */

int kahramanları_dosyadan_oku(const char* filename, char* buffer, size_t size) {
    FILE *heroes = fopen(filename, "r");
    if (heroes == NULL) {
        printf("Acilamayan dosya: %s!\n", filename);
        return 1;
    }
    size_t heroes_json_length = fread(buffer, sizeof(char), size - 1, heroes);
    buffer[heroes_json_length] = '\0';
    fclose(heroes);
    return 0;
}

void kahraman_bonus_degeri(char* start, char* end, struct hero* Hero) {
    char* bonus_str = strstr(start, "bonus_degeri");
    if (bonus_str && bonus_str < end) {
        char* colon = strchr(bonus_str, ':') + 1;
        while (*colon == ' ' || *colon == '"') {
            colon++;
        }
        char* end_pointer = colon;
        while (*end_pointer != ',' && *end_pointer != '}' && *end_pointer != '"') {
            end_pointer++;
        }
        int uzunluk = end_pointer - colon;
        char* deger = malloc(uzunluk + 1);
        strncpy(deger, colon, uzunluk);
        deger[uzunluk] = '\0';
        Hero->etki[0] = atoi(deger);
        free(deger);
    }
}

void kahraman_etkiledigi_birim(char* start, char* end, struct hero* Hero) {
    for (int i = 0; i < 9; i++) {
        char* bulunan = strstr(start, birimAdlari[i]);
        if (bulunan && bulunan < end) {
            strcpy(Hero->etkiledigi_birim, birimAdlari[i]);
            Hero->etkiledigi_index = i;
            return;
        }
    }
    strcpy(Hero->etkiledigi_birim, "yok");
}

void kahraman_deger_oku(char* ilk_pointer, struct hero* Hero) {
    if (!ilk_pointer) {
        strcpy(Hero->etkiledigi_birim, "yok");
        return;
    }

    char* son_pointer = strchr(ilk_pointer, '}');
    if (son_pointer) {
        kahraman_bonus_degeri(ilk_pointer, son_pointer, Hero);
        kahraman_etkiledigi_birim(ilk_pointer, son_pointer, Hero);
    }
}

int birimDegerleriOku(char* ilk_pointer, char* anahtar){
    char* bulunan;
    char* bulunan_son;
    char* son_pointer=ilk_pointer;
    while (*son_pointer!='}')
    {
        son_pointer++;
    }

    bulunan=strstr(ilk_pointer,anahtar);
    if (bulunan>son_pointer)
    {
        return 0;
    }
    else
    {
        bulunan=strchr(bulunan,':');
        bulunan++;
        while (*bulunan==' ' || *bulunan=='"')
        {
            bulunan++;
        }
        bulunan_son=bulunan;

        while (*bulunan_son!=',' && *bulunan_son!='}' && *bulunan_son!='"')
        {
            bulunan_son++;
        }

        int uzunluk=bulunan_son-bulunan;
        char* deger=malloc(uzunluk+1);
        strncpy(deger,bulunan,uzunluk);
        deger[uzunluk]='\0';
        return atoi(deger);
    }
}

int parse_bonus_deger(const char* json_fragment) {
    const char* pos = strstr(json_fragment, "etki_degeri");
    if (!pos) return 0;

    pos = strchr(pos, ':');
    return pos ? atoi(pos + 1) : 0;
}

void parse_etki_turu(const char* json_fragment, struct creature* Creature, int bonus_deger) {
    for (int i = 0; i < 3; i++) {
        const char* pos = strstr(json_fragment, kahmaranCanavarKeyleri[i]);
        if (pos) {
            Creature->etki[i] = bonus_deger;
        }
    }
}

void yaratiklarin_degerlerini_bul(const char* json_fragment, struct creature* Creature) {
    int bonus_deger = parse_bonus_deger(json_fragment);

    // "etkiledigi_birim" kontrolü
    strcpy(Creature->etkiledigi_birim, "yok");
    Creature->etkiledigi_index = -1;
    for (int i = 0; i < 5; i++) {
        const char* pos = strstr(json_fragment, insanYaratikAdlari[i]);
        if (pos) {
            strcpy(Creature->etkiledigi_birim, insanYaratikAdlari[i]);
            Creature->etkiledigi_index = i;
            break;
        }
    }

    // "etki_turu" işleme
    parse_etki_turu(json_fragment, Creature, bonus_deger);
}


void arastirma_degerlerini_bul(const char* arastirma_string, const char* seviye, struct research* Research) {
    char* seviye_pointer = strstr(arastirma_string, seviye);
    if (seviye_pointer == NULL) return;  // Eğer seviye bulunamazsa, fonksiyondan çık

    char* deger_pointer = strstr(seviye_pointer, "deger");
    if (deger_pointer == NULL) return;  // Eğer değer bulunamazsa, fonksiyondan çık

    deger_pointer = strchr(deger_pointer, ':');  // ':' karakterini bul
    if (deger_pointer == NULL) return;
    deger_pointer++;  // ':' karakterinden sonrasına geç

    while (*deger_pointer == ' ' || *deger_pointer == '"') {
        deger_pointer++;  // Boşlukları veya tırnakları geç
    }

    char* deger_son = deger_pointer;
    while (*deger_son != ',' && *deger_son != '}' && *deger_son != '"') {
        deger_son++;  // Değerin sonuna kadar ilerle
    }

    int uzunluk = deger_son - deger_pointer;
    char deger[uzunluk + 1];
    strncpy(deger, deger_pointer, uzunluk);
    deger[uzunluk] = '\0';

    Research->etki = atoi(deger);

    // Birimi ve etki indeksini ayarlamak
    char* son_bracket = strchr(seviye_pointer, '}');
    if (son_bracket == NULL) return;

    Research->etkiledigi_birim[0] = '\0';  // Etkilediği birim için boş bir string ayarla
    for (int i = 0; i < 9; i++) {
        char* birim_pointer = strstr(seviye_pointer, birimAdlari[i]);
        if (birim_pointer && birim_pointer < son_bracket) {
            strcpy(Research->etkiledigi_birim, birimAdlari[i]);
            Research->etkiledigi_index = i;
            break;
        }
    }
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total_size = size * nmemb;
    char *buffer = (char *)userp;

    // Gelen veriyi buffer'a ekle
    strncat(buffer, contents, total_size);

    return total_size;
}
void senaryodan_deger_oku(char* ilk_pointer, struct birim birimler[], struct hero Heros[], struct creature Creatures[], struct research Arastirma[][3])
{
    int uzunluk;
    char* deger;
    char* bulunan;
    char* bulunan_son;
    char* son_pointer=ilk_pointer;
    while (*son_pointer!='}')
    {
        son_pointer++;
    }
    son_pointer++;
    while (*son_pointer!='}')
    {
        son_pointer++;
    }
    son_pointer++;
    while (*son_pointer!='}')
    {
        son_pointer++;
    }
    for (int i=0;i<8;i++)
    {
            bulunan=strstr(ilk_pointer,birimAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                bulunan=strchr(bulunan,':');
                bulunan++;
                while (*bulunan==' ' || *bulunan=='"')
                {
                    bulunan++;
                }
                bulunan_son=bulunan;
                while (*bulunan_son!=',' && *bulunan_son!='}' && *bulunan_son!='"')
                {
                    bulunan_son++;
                }
                uzunluk=bulunan_son-bulunan;
                deger=malloc(uzunluk+1);
                strncpy(deger,bulunan,uzunluk);
                deger[uzunluk]='\0';
                birimler[i].birim_sayisi=atoi(deger);
                birimler[i].ilk_birim_sayisi=atoi(deger);
            }
            else
            {
                continue;
            }
    }
    for (int i=0;i<5;i++)
    {
            bulunan=strstr(ilk_pointer,insanKahramanAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                Heros[i].varmi=true;
            }
            else
            {
                continue;
            }
    }
    for (int i=0;i<4;i++)
    {
            bulunan=strstr(ilk_pointer,orkKahrarmanAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                Heros[i].varmi=true;
            }
            else
            {
                continue;
            }
    }
    for (int i=0;i<5;i++)
    {
            bulunan=strstr(ilk_pointer,insanYaratikAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                Creatures[i].varmi=true;
            }
            else
            {
                continue;
            }
    }
    for (int i=0;i<6;i++)
    {
            bulunan=strstr(ilk_pointer,orkYaratikAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                Creatures[i].varmi=true;
            }
            else
            {
                continue;
            }
    }
    for (int i=0;i<4;i++)
    {
            bulunan=strstr(ilk_pointer,arastirmaAdlari[i]);
            if (bulunan==NULL)
            {
                continue;
            }
            else if (bulunan<son_pointer)
            {
                bulunan=strchr(bulunan,':');
                bulunan++;
                while (*bulunan==' ' || *bulunan=='"')
                {
                    bulunan++;
                }
                bulunan_son=bulunan;
                while (*bulunan_son!=',' && *bulunan_son!='}' && *bulunan_son!='"')
                {
                    bulunan_son++;
                }
                uzunluk=bulunan_son-bulunan;
                deger=malloc(uzunluk+1);
                strncpy(deger,bulunan,uzunluk);
                deger[uzunluk]='\0';
                Arastirma[i][atoi(deger)-1].varmi=true;
            }
            else
            {
                continue;
            }
    }
}

int saldiri_gucu_hesapla(struct birim Unit, int birim_index, struct creature Creatures[], struct hero Heros[], struct research Arastirma[][3], int hero_sayisi, int creature_sayisi, int tur)
{
    if (Unit.birim_sayisi==0)
    {
        return 0;
    }
    float toplam_kritik_sans=Unit.kritik_sans;
    int saldiri_gucu=Unit.saldiri*Unit.birim_sayisi;
    for (int i=0;i<hero_sayisi;i++)
    {
        if (Heros[i].varmi==1 && (Heros[i].etkiledigi_index==birim_index || Heros[i].etkiledigi_index==8))
        {
            saldiri_gucu=(float)(saldiri_gucu*(1.0+(Heros[i].etki[0]/100.0)));
            toplam_kritik_sans=toplam_kritik_sans*(1.0+(Heros[i].etki[2]/100.0));
        }
    }
    for (int i=0;i<creature_sayisi;i++)
    {
        if (Creatures[i].varmi==1 && (Creatures[i].etkiledigi_index==birim_index || Creatures[i].etkiledigi_index==8))
        {
            saldiri_gucu=(float)(saldiri_gucu*(1.0+(Creatures[i].etki[0]/100.0)));
            toplam_kritik_sans=toplam_kritik_sans*(1.0+(Creatures[i].etki[2]/100.0));
        }
    }
    for (int i=0;i<3;i++)
    {
        if (Arastirma[1][i].varmi==1 && (Arastirma[1][i].etkiledigi_index==birim_index || Arastirma[1][i].etkiledigi_index==8))
        {
            saldiri_gucu=(float)(saldiri_gucu*(1.0+(Arastirma[1][i].etki/100.0)));
        }
    }
    for (int i=0;i<3;i++)
    {
        if (Arastirma[2][i].varmi==1 && (Arastirma[2][i].etkiledigi_index==birim_index || Arastirma[2][i].etkiledigi_index==8))
        {
            toplam_kritik_sans=toplam_kritik_sans*(1.0+(Arastirma[2][i].etki/100.0));
        }
    }
    if (toplam_kritik_sans!=0 && tur%(100/(int)toplam_kritik_sans)==0)
    {
        saldiri_gucu=((float)saldiri_gucu*(float)1.5);
    }
    if (saldiri_gucu<0)
    {
        saldiri_gucu=0;
    }

    int yorgunluk_sayisi=tur/5;

    for (int i=0;i<yorgunluk_sayisi;i++)
    {
        saldiri_gucu=(float)saldiri_gucu*(float)(0.9);
    }

    return saldiri_gucu;
}

int savunma_gucu_hesapla(struct birim Unit, int birim_index, struct creature Creatures[], struct hero Heros[], struct research Arastirma[][3], int hero_sayisi, int creature_sayisi, int tur)
{
    if (Unit.birim_sayisi==0)
    {
        return 0;
    }
    int savunma_gucu=Unit.savunma*Unit.birim_sayisi;
    for (int i=0;i<hero_sayisi;i++)
    {
        if (Heros[i].varmi==1 && (Heros[i].etkiledigi_index==birim_index || Heros[i].etkiledigi_index==8))
        {
            savunma_gucu=(float)(savunma_gucu*(1.0+(Heros[i].etki[1]/100.0)));
        }
    }
    for (int i=0;i<creature_sayisi;i++)
    {
        if (Creatures[i].varmi==1 && (Creatures[i].etkiledigi_index==birim_index || Creatures[i].etkiledigi_index==8))
        {
            savunma_gucu=(float)(savunma_gucu*(1.0+(Creatures[i].etki[1]/100.0)));
        }
    }
    for (int i=0;i<3;i++)
    {
        if (Arastirma[0][i].varmi==1 && (Arastirma[0][i].etkiledigi_index==birim_index || Arastirma[0][i].etkiledigi_index==8))
        {
            savunma_gucu=(float)(savunma_gucu*(1.0+(Arastirma[0][i].etki/100.0)));
        }
    }
    int yorgunluk_sayisi=tur/5;

    for (int i=0;i<yorgunluk_sayisi;i++)
    {
        savunma_gucu=(float)savunma_gucu*(float)0.9;
    }
    if (savunma_gucu<0)
    {
        savunma_gucu=0;
    }
    return savunma_gucu;
}

void saldiri_etkisi_hesapla(int dusman_saldiri_gucu, int toplam_savunma_gucu, struct birim* Unit)
{
    int net_hasar;
    if (toplam_savunma_gucu>dusman_saldiri_gucu)
    {
        net_hasar=0;
    }
    else
    {
        net_hasar=(float)dusman_saldiri_gucu*(1.0-(float)toplam_savunma_gucu/(float)dusman_saldiri_gucu);
    }
    float savunma_orani=(float)Unit->savunma_gucu/toplam_savunma_gucu;
    int verilen_hasar=savunma_orani*(float)net_hasar;
    Unit->toplam_saglik=Unit->toplam_saglik-verilen_hasar;
    if (Unit->toplam_saglik<0)
    {
        Unit->toplam_saglik=0;
    }
    if (Unit->toplam_saglik<Unit->saglik && Unit->toplam_saglik!=0)
    {
        Unit->birim_sayisi=1;
    }
    else
    {
        Unit->birim_sayisi=Unit->toplam_saglik/Unit->saglik;
    }
}

void randomYerlestir(struct birim Unit, int birim_index, struct Cell ekran[][26])
{

    int birim_sayisi=Unit.birim_sayisi;
    while (birim_sayisi!=0)
    {
        if (birim_index<4)
        {
            int i=rand()%(SUTUN/2);
            int j=rand()%(SATIR-6);
            if (ekran[i][j].hasUnit==true)
            {
                continue;
            }
            if (birim_sayisi>=100 && ekran[i][j].isVisible==true && ekran[i][j].hasUnit==false)
            {
                ekran[i][j].birim_sayisi=100;
                birim_sayisi=birim_sayisi-100;
                ekran[i][j].birim_index=birim_index;
                ekran[i][j].hasUnit=true;
            }
            else if (birim_sayisi<100 && ekran[i][j].isVisible==true && ekran[i][j].hasUnit==false)
            {
                ekran[i][j].birim_sayisi=birim_sayisi;
                ekran[i][j].birim_index=birim_index;
                birim_sayisi=0;
                ekran[i][j].hasUnit=true;
            }
        }
        else if (birim_index>=4)
        {
            int i=rand()%(SUTUN/2)+10;
            int j=rand()%(SATIR-6);
            if (ekran[i][j].hasUnit==true)
            {
                continue;
            }
            if (birim_sayisi>=100 && ekran[i][j].isVisible==true && ekran[i][j].hasUnit==false)
            {
                ekran[i][j].birim_sayisi=100;
                birim_sayisi=birim_sayisi-100;
                ekran[i][j].birim_index=birim_index;
                ekran[i][j].hasUnit=true;
            }
            else if (birim_sayisi<100 && ekran[i][j].isVisible==true && ekran[i][j].hasUnit==false)
            {
                ekran[i][j].birim_sayisi=birim_sayisi;
                ekran[i][j].birim_index=birim_index;
                birim_sayisi=0;
                ekran[i][j].hasUnit=true;
            }
        }
    }

}

void draw_cell(struct Cell ekran, int i, int j)
{
    struct Color color;

    if (ekran.isVisible)
    {
        // Sol taraftaki hücreler için
        if (j < 14)
        {
            DrawRectangleLines(i * hucreEni, j * hucreBoy, hucreEni, hucreBoy, DARKGREEN);

            for (int k = 0; k < 4; k++)
            {
                if (ekran.birim_index == k && ekran.birim_sayisi != 0)
                {
                    Rectangle source = {0, 0, birimGorsel[k].width, birimGorsel[k].height};
                    Rectangle dest = {i * hucreEni, j * hucreBoy, hucreEni, hucreBoy};
                    Vector2 origin = {0, 0};
                    DrawTexturePro(birimGorsel[k], source, dest, origin, 0.0f, WHITE);

                    char birim_sayisi[5];  // 'malloc' yerine statik dizi
                    snprintf(birim_sayisi, sizeof(birim_sayisi), "%d", ekran.birim_sayisi);

                    DrawRectangle((hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni - 3, j * hucreBoy + hucreBoy - 21, MeasureText(birim_sayisi, 20) + 6, 22, BLACK);

                    if (ekran.birim_sayisi >= 80)
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, LIME);
                    }
                    else if (ekran.birim_sayisi < 80 && ekran.birim_sayisi >= 30)
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, BLACK);
                    }
                    else
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, RED);
                    }
                }
            }
        }
        // Sağ taraftaki hücreler için
        else
        {
            DrawRectangleLines(i * hucreEni, j * hucreBoy, hucreEni, hucreBoy, RED);

            for (int k = 4; k < 8; k++)
            {
                if (ekran.birim_index == k && ekran.birim_sayisi != 0)
                {
                    Rectangle source = {0, 0, birimGorsel[k].width, birimGorsel[k].height};
                    Rectangle dest = {i * hucreEni, j * hucreBoy, hucreEni, hucreBoy};
                    Vector2 origin = {0, 0};
                    DrawTexturePro(birimGorsel[k], source, dest, origin, 0.0f, WHITE);

                    char birim_sayisi[5];  // 'malloc' yerine statik dizi
                    snprintf(birim_sayisi, sizeof(birim_sayisi), "%d", ekran.birim_sayisi);

                    DrawRectangle((hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni - 3, j * hucreBoy + hucreBoy - 21, MeasureText(birim_sayisi, 20) + 6, 22, BLACK);

                    if (ekran.birim_sayisi >= 80)
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, LIME);
                    }
                    else if (ekran.birim_sayisi < 80 && ekran.birim_sayisi >= 30)
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, ORANGE);
                    }
                    else
                    {
                        DrawText(birim_sayisi, (hucreEni - MeasureText(birim_sayisi, 20)) / 2 + i * hucreEni, j * hucreBoy + hucreBoy - 19, 20, RED);
                    }
                }
            }
        }
    }
}


void draw_toplam_gucler(int insan_toplam_savunma, int insan_toplam_saldiri, int ork_toplam_savunma, int ork_toplam_saldiri)
{
    char insan_toplam_saldiri_string[50];
    char insan_toplam_savunma_string[50];
    sprintf(insan_toplam_saldiri_string,"Insanlar Toplam Saldiri: %d",insan_toplam_saldiri);
    sprintf(insan_toplam_savunma_string,"Insanlar Toplam Savunma: %d",insan_toplam_savunma);

    DrawText(insan_toplam_saldiri_string,4*hucreEni+3,22*hucreBoy+hucreBoy/2-9,18,GREEN);
    DrawText(insan_toplam_savunma_string,4*hucreEni+3,23*hucreBoy+hucreBoy/2-9,18,GREEN);

    char ork_toplam_saldiri_string[50];
    char ork_toplam_savunma_string[50];
    sprintf(ork_toplam_saldiri_string,"Org Imp. Toplam Saldiri: %d",ork_toplam_saldiri);
    sprintf(ork_toplam_savunma_string,"Org Imp. Toplam Savunma: %d",ork_toplam_savunma);
    DrawText(ork_toplam_saldiri_string,10*hucreEni+3,22*hucreBoy+hucreBoy/2-9,18,RED);
    DrawText(ork_toplam_savunma_string,10*hucreEni+3,23*hucreBoy+hucreBoy/2-9,18,RED);
}



