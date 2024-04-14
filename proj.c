#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/*Note: on considere dans ce programme que toutes chaines de caracteres fait moins de 100 caracteres
i.e. le nom du fichier source, les chaines de codes, les chaines traduites...*/

/* On creer une structure etiq qui representera toutes les etiquettes de notre code, par 
leurs noms et leur numero de ligne dans le code (place)*/

struct e{
	char* nom;
	int place;
	};
	
typedef struct e etiq;


/* On creer une structure instruction qui representera toutes les instructions à executer, par la première instruction et l'eventuelle donnee*/

struct Instruction{
	int instru;
	int donnee;
	};

typedef struct Instruction instruction;


//initialisation du tableau de toutes les instructions possibles//

char* instructions[15] = {"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};		
												
																																
												
/*traduction d'une ligne de code source en code machine (cas 1)
cpt designe la ligne de chaine consideree, nescessaire pour le compteur
T est un tableau de pointeur sur etiquettes, avec toutes les etiquettes dans le tableau
debdonne est le nombre de caractere a partir duquel l'instruction de la ligne devrait suivre*/

char* traduction1(char* chaine,int cpt, etiq** T){				
		
//traduction du premier octet//							
	int e=0;
	for(int i=0; i<strlen(chaine);i++){							//on regarde si notre chaine commence avec ou sans etiquette//
		if (chaine[i]==':')					
			e=1;								
		}
	
	int i=0;	
	//si notre chaine commence avec une etiquette//
	if (e){							
		while(chaine[i]!=':')
			i++;
		i++;
		while(chaine[i]==' ' || chaine[i]=='\t')
			i++;
		}
	else{
		while (chaine[i]==' ' || chaine[i]=='\t')
			i++;
		}
				
	//copie de l'instruction dans instru//			
	char instru[100];									//instruction est de taille maximum 5//
	int k=0;
	while(chaine[i]!='\0' && chaine[i]!=' ' && chaine[i]!='\t'){
		instru[k]=chaine[i];
		i++;
		k++;
		}
	instru[k]='\0';
		
	//on traduit l'instruction en hexadecimal//
	static char traduit[100];
	int j=0;
	while(j<15 && strcmp(instru,instructions[j]))		
		j++;
	if (j>=15)										//sans oublier les cas d'erreurs//
		return "erreur";
	traduit[0]='0';										//on traduit l'instruction en hexa//
	char tmp[2];
	sprintf(tmp,"%x",j);
	traduit[1]=tmp[0];
	traduit[2] = ' ';
	


//traduction des quatre autres octets//
	
	//si operations sans donnees//
	if (j==2 || j==4 || j==5 || j==13 || j==14){
		while (chaine[i]==' ' || chaine[i]=='\t')
			i++;
		if (chaine[i]!= '\0')
			return "erreur";
		for (int k=3;k<11;k++)
			traduit[k]='0';
		}
			
	else{
		i++;
		int hexa=1000;									//nombre trop grand pour que ce soit vraiment une etiquette dans le programme, si hexa vaut toujours 1000, il y a forcément un problème.//
		char donnee[100];
		k=0;
		
		//operations suivies d'une etiquette//
		if (j==7 || j==8 || j==12){			
			while(chaine[i]!='\0' && chaine[i]!=' ' && chaine[i]!='\t'){		//on convertit l'etiquette en un chiffre en base 10
				donnee[k]=chaine[i];
				i++;
				k++;
				}
			donnee[k] = '\0';
			while((*T)->place != -1){
				if (!strcmp((*(T++))->nom,donnee)){
					hexa = (*(--T))->place - cpt;
					break;
					}
				}
				if 	(hexa==1000)						// étiquette non trouvée dans la liste d'étiquettes initalisées
					return "erreur";
			}
		//operations suivies d'une donnee int//
		else{
			while(chaine[i]!='\0' && chaine[i]!=' ' && chaine[i]!='\t'){
				if(chaine[i]<'0' || chaine[i]>'9')
					return "erreur";
				donnee[k++]=chaine[i++];
				}
			hexa = atoi(donnee);
			if(j==6 && (hexa<0 || hexa>15))
				return "erreur";
			else{
				if (hexa>pow(16,7) || hexa<(-(pow(16,7)-1)))			// donnée trop grande à traduire
					return "erreur";
				}
			}	
		while (chaine[i]==' ' || chaine[i]=='\t')
			i++;
		if (chaine[i]!= '\0')
			return "erreur";
		sprintf(donnee,"%x",hexa);	
		for (int k=3;k<11-strlen(donnee);k++)
			traduit[k] = '0';
		for (int k=11-strlen(donnee);k<12;k++)
			traduit[k]=donnee[k-(11-strlen(donnee))];
		}
	return traduit;
	}
		
		
int main (int argc, char* argv[]){
	if (argc!=2){
		printf("Le programme n'a pas récuperer un fichier en arguement, veuillez recommencer.\n");
		return 0;
		}
	FILE* fsource = fopen(argv[1],"r");
	if (fsource == NULL){									//erreur dans l'ouverture du fichier//
		printf("Erreur dans l'ouverture du fichier, veuillez relancer le programme.\n");
		return 0;
		}
	char chaine[100];
	
	if (fgets(chaine, 100, fsource)==NULL){							//code source vide//
		fclose(fsource);
		printf("Fichier source vide, aucune traduction possible.\n");
		return 0;}
	
	etiq** Tab = malloc(sizeof(etiq*)*100);							//100 choisit comme le nombre d'etiquette maximale dans le code source//
	etiq** init = Tab;									//init garde en memoire la premiere adresse de notre tableau d'etiquettes
	int cpt = 0;										//cpt designe le numéro de la ligne dans le code source//
	char name[100];
	do{
		int i=0;
		int e=0;
		while(chaine[i]!='\0' && chaine[i]!=':'){
			name[i]=chaine[i];
			i++;
			if (chaine[i]==':')
				e=1;
			}
		if (e==1){
			name[i]='\0';
			etiq* nvetiq=malloc(sizeof(etiq));
			nvetiq->nom = strdup(name);
			nvetiq->place = cpt;
	 		*Tab = nvetiq;
	 		Tab++;
			}
		cpt++;
	}while (fgets(chaine, 100, fsource)!=NULL);
	int nligne = cpt;									//on garde le nombre de lignes dans le fichier pour plus tard
	etiq* fin = malloc(sizeof(etiq));
	fin->place = -1;									//sert a determiner la fin de notre tableau d'étiquettes
	(*Tab) = fin;
	Tab = init;
	for (int i = 0;(*(Tab+i))->place!=-1;i++){						//on vérifie que deux étiquettes n'ont pas le meme nom
		for (int j = 1;(*(Tab+i+j))->place!=-1;j++){		
			if (!strcmp((*(Tab+i+j))->nom,(*(Tab+i))->nom)){
				printf("une etiquette a été initalisée a deux endroits différents, veuillez relancez\n");
				return 0;
				}
			}
		}
	fclose(fsource);
	
					
	//ecriture dans le nouveau fichier//
	fsource = fopen(argv[1],"r");
	FILE* fexec = fopen("hexa.txt","w");
	char ligne[100];
	cpt = 1;
	Tab = init;
	while (fgets(ligne, 100, fsource)!=NULL){
		char lignetrad[100];
		//if (cpt!=nligne)			<- Instruction eventuellement à mettre, qui porte uniquement sur la ligne suivante (sur certains ordinateurs, la dernire ligne finissait par un '\n', sur certains autres non.							
			ligne[strlen(ligne)-1] = '\0';							
		strcpy(lignetrad,traduction1(ligne, cpt, Tab));
		if (!strcmp("erreur",lignetrad)){
			printf("erreur de syntaxe dans la ligne numéro %d.\n",cpt-1);
			return 0;
			}
		if (cpt!=nligne)
			lignetrad[strlen(lignetrad)]='\n';
		fputs(lignetrad,fexec);
		cpt++;
		}
	fclose(fsource);
	fclose(fexec);
	
	int i=0;										// On free notre tableau d'etiquettes.
	for (i=0;Tab[i]->place!=-1;i++){
		free(Tab[i]->nom);
		free(Tab[i]);
		}
	free(Tab[i]->nom);
	free(Tab[i]);
	free(Tab);
	
	//Le fichier hexa.txt a bien été généré, on passe a son exécution. 
	FILE* f = fopen("hexa.txt","r");

	instruction** p = malloc(sizeof(instruction*)*nligne);					//On initialise p comme tableau d'instructions.
	char tmp[20]; char tmpi[2]; char tmpd[10];
	while (fgets(tmp,20,f)){
		instruction* inst = malloc(sizeof(instruction));
		tmpi[0] = tmp[1];
		for (int k=3;k<11;k++)
			tmpd[k-3]=tmp[k];
		inst->instru = strtol(tmpi,NULL,16);
		inst->donnee = strtol(tmpd,NULL,16);
		*(p++) = inst;
		}
	p-=nligne;
	
	int memoire[5000];									//On initalise notre memoire qui contient 5000 adresses sur quatre octet, donc des int.
	int SP = 0;	
		int d; int PC = 0; int val; int alea;

	while(PC<nligne){									//On exécute le programme.
		i = (*(p+PC))->instru;
		d = (*(p+PC))->donnee;
		if (SP<0 || SP>4999){								//On présente un premier cas général, qui empêche la possibilitée que SP soit hors zone mémoire.
			printf("Erreur de segmentation (core dumped).\n");
			return 0;
			}
		if (i==0){
			if (d<0 || d>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[SP++] = memoire[d];
			}
		if (i==1)
			memoire[SP++] = d;
		if (i==2){
			if (SP==0 || memoire[SP-1]<0 || memoire[SP-1]>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[SP-1] = memoire[memoire[SP-1]];
			}
		if (i==3){
			if (d<0 || d>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[d] = memoire[--SP];
			}
		if (i==4){
			if (SP<2 || memoire[SP-1]<0 || memoire[SP-1]>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[memoire[SP-1]] = memoire[SP-2];
			SP-=2;
			}
		if (i==5){
			if (SP==0){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[SP] = memoire[SP-1];
			SP++;
			}
		if (i==6){									//On énumere les différentes conditions des op.
			if (SP<2 || (SP == 1 && (d!=15 && d!=5))){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			if(d==0){
				SP--;
				memoire[SP-1]=memoire[SP-1]+memoire[SP];
				}
			if(d==1){
				SP--;
				memoire[SP-1]=memoire[SP-1]-memoire[SP];
				}				
			if(d==2){
				SP--;
				memoire[SP-1]=memoire[SP-1]*memoire[SP];
				}
			if(d==3){
				SP--;
				if (memoire[SP]==0){
					printf("Erreur, division par 0 à la ligne %d.\n",PC);
					return 0;
					}
				memoire[SP-1]=memoire[SP-1]/memoire[SP];
				}
			if(d==4){
				SP--;
				if (memoire[SP]==0){
					printf("Erreur, division par 0 à la ligne %d.\n",PC);
					return 0;
					}
				memoire[SP-1]=memoire[SP-1]%memoire[SP];
				}
			if(d==5)
				memoire[SP-1]=-memoire[SP-1];
			if(d==6){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]==memoire[SP]);
				}
			if(d==7){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]!=memoire[SP]);
				}			
			if(d==8){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]>memoire[SP]);
				}
			if(d==9){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]>=memoire[SP]);
				}
			if(d==10){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]<memoire[SP]);
				}
			if(d==11){
				SP--;
				memoire[SP-1] = !(memoire[SP-1]<=memoire[SP]);
				}
			if(d==12){
				SP--;
				memoire[SP-1] = memoire[SP-1]&memoire[SP];
				}
			if(d==13){
				SP--;
				memoire[SP-1] = memoire[SP-1]|memoire[SP];
				}
			if(d==14){
				SP--;
				memoire[SP-1] = memoire[SP-1]^memoire[SP];
				}
			if(d==15)
				memoire[SP-1] = ~memoire[SP-1];
			}
		if (i==7)
			PC+=d;
		if (i==8){
			if (SP==0){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			if (memoire[--SP]==0)
				PC+=d;
			}
		if (i==9){
			srand(time(NULL));
			alea = rand() % (d-1);
			memoire[SP++] = alea;
			}
		if (i==10){
			if (d<0 || d>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			printf("Veuillez rentrer une valeur.\n");
			scanf("%d",&val);
			memoire[d] = val;
			}
		if (i==11){
			if (d<0 || d>4999){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			printf("%d\n",memoire[d]);
			}
		if (i==12){
			if (SP==5000){
				printf("Erreur de segmentation (core dumped).\n");
				return 0;
				}
			memoire[SP++] = PC;
			PC+=d;
			}
		if (i==13)
			PC = memoire[SP--];
		if (i==14){
			for (i=0;i<nligne;i++)						//On free notre tableau d'instructions.
				free(p[i]);
			free(p);
			return 0;
			}
		PC++;
		}
	printf("Erreur: votre programme a dépassé la dernière ligne sans aucune instruction halt.\n");
	return 0;
	}	

			 	
		
		
			
		




