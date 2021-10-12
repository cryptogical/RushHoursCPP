#include <cstdio>
#include <string.h>
#include <time.h>
// NOT MY WORK !
#define large 6 		// largeur du jeu
#define write "%.6s\n"	// affichage d'une ligne
#define haut  6			// hauteur du jeu
#define goaly 2			//ligne de sortie du céhicule
/*
#define large 5
#define write "%.5s\n"
#define haut  5
#define goaly 2
*/

#define large1 large-1	// -1 pour véhicule de longueur 2
#define large2 large-2	// -2 pour véhicule de longueur 3 
#define haut1 haut-1 	// -1 pour véhicule de longueur 2
#define haut2 haut-2  	// -2 pour véhicule de longueur 3
#define entier large*haut	// jeu complet

class vehicle
{
	char nom;//le "nom" du véhicule
	bool V;//Vertical ou Horizontal
	char x;//la colonne où va rester le véhicule Vertical
	char y;//la ligne où va rester le véhicule Horizontal
	char len;//la longueur du véhicule
public:
	vehicle(char name,bool Ve,char xx,char yy,char lon):nom(name),V(Ve),x(xx),y(yy),len(lon){}
	char getnom(){return nom;}
	bool getV(){return V;}
	char getx(){return x;}
	char gety(){return y;}
	char getlen(){return len;}
};

class jeu
{
	//union{
	char je[haut][large];//le jeu sous forme d'un tableau de char
	//char ch[entier];};
public:
	char getje(char y,char x){return je[y][x];}
	jeu(char*s)//constructeur avec des "véhicules" de A à X et des case vides de '.'
	{
		memcpy(je,s,entier);
	}
	jeu(jeu*jj)//constructeur copie
	{
		memcpy(je,jj,entier);
	}
	void print()
	{//on "écrit" le jeu à l'écran
		for(char y=0;y<haut;y++)
		{
			printf(write,je[y]);
		}
	}
	bool goal()//la voiture XX peut-elle sortir par la droite ?
	{
		for(char x=large1;x>0;x--)
		{
			if(je[goaly][x]=='X')return true;
			if(je[goaly][x]!='.')return false;
		}
		return false;
	}
	char getx(char c,char y)
	{
		for(char n=0;n<large;n++)if(je[y][n]==c)return n;
	}
	char gety(char c,char x)
	{
		for(char n=0;n<haut;n++)if(je[n][x]==c)return n;
	}
	char mouveg(char x,char y)
	{//retourne le nombre de mouvement à gauche
		char g=0;
		for(char n=x-1;n>=0;n--)if(je[y][n]=='.')g++;else return g;
		return g;
	}
	char mouved(char x,char y,char len)
	{//retourne le nombre de mouvement à droite
		char d=0;
		for(char n=x+len;n<large;n++)if(je[y][n]=='.')d++;else return d;
		return d;
	}
	char mouveh(char x,char y)
	{//retourne le nombre de mouvement en haut
		char h=0;
		for(char n=y-1;n>=0;n--)if(je[n][x]=='.')h++;else return h;
		return h;
	}
	char mouveb(char x,char y,char len)
	{//retourne le nombre de mouvement en bas
		char b=0;
		for(char n=y+len;n<haut;n++)if(je[n][x]=='.')b++;else return b;
		return b;
	}
	void bougeH(char c,char x,char y,char len,char m)
	{//bouge un "véhicule" de m places à l'horizontal
		for(char n=0;n<len;n++)je[y][x+n]='.';
		for(char n=0;n<len;n++)je[y][x+n+m]=c;
	}
	void bougeV(char c,char x,char y,char len,char m)
	{//bouge un "véhicule" de m places en vertical
		for(char n=0;n<len;n++)je[y+n][x]='.';
		for(char n=0;n<len;n++)je[y+n+m][x]=c;
	}
};

char noms[]={//tous les noms possibles des véhicules. (Le . désigne une case vide)
'0','1','2','3','4','5','6','7','8','9',
'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',0
};
class srush;//provisoirement
srush*ici;//le jeu de base (au début)
srush*sol[60];// toute la solution
jeu*mesjeux[400000];//pour stocker les différents jeux
int nbj=0;//le nombre de jeux stockés dans mesjeux (positions différentes analysées)
vehicle*vehicles[entier/2];//tous les véhicules du jeu
int nbrpositions=0;//nombre de positions analysées au final
char nbre;//nombre de véhicules
jeu j(noms);//pour bouger une position et la tester (noms ne sert à rien, c'est char[entier] et +)

inline bool notdeja()//retourne true si cette position n'existe pas déjà
{
	for(int n=nbj-1;n>=0;n--)//on essaye toutes les positions précédentes
	{//on commence par la fin "plus proche" de la position en cours
		if(memcmp(mesjeux[n],&j,entier)==0)return false;
	}
	return true;
}

class srush
{
	jeu*io;//le jeu
	char txt[4];//le mouvement
	char niveau;//profondeur de recherche
	srush*parent;//le jeu parent dont celui-ci est issu
	srush*frere;//toute les possibilités de mouvements (les fils)
public:
	srush(char*s):parent(0),frere(0),niveau(0)//construction du jeu avec une String (voir la class jeu) et mise en place des "véhicules"
	{
		char c;//un véhicule
		io=new jeu(s);
		nbre=0;
		for(char cc=0;c=noms[cc];cc++)//on essaye tous les "véhicules" possibles à partir des noms[]
			for(char y=0;y<haut;y++)//pour toutes les lignes
				for(char x=0;x<large;x++)//pour toute la ligne
					if(io->getje(y,x)==c)//on trouve un véhicule
					{
						if((x<large1)&&(io->getje(y,x+1)==c))
						{//H
							vehicles[nbre++]=new vehicle(c,false,x,y,(x<large2)&&(io->getje(y,x+2)==c)?3:2);
						}
						else if((y<haut1)&&(io->getje(y+1,x)==c))
						{//V
							vehicles[nbre++]=new vehicle(c,true,x,y,(y<haut2)&&(io->getje(y+2,x)==c)?3:2);
						}//else erreur
						x=large;
						y=haut;//véhicule suivant
					}
		mesjeux[nbj++]=io;//on stocke le jeu de départ
	}
	srush(jeu*s):frere(0)//construction du jeu avec un jeu s
	{
		io=new jeu(s);
	}
	~srush()
	{
		if(ici==this)for(char n=0;n<nbre;n++)delete vehicles[n];
		if(io)delete io;
		if(frere)delete frere;
	}
	void print()
	{//on écrit le jeu "à l'écran"
		printf("------ %u %s\n",niveau,txt);
		io->print();
	}
	void solution()
	{
		sol[niveau]=this;
		if(parent)parent->solution();
		else *txt=0;
	}
	bool test(char niv)//on "teste" le niveau
	{
		if(run(niv))return true;//solution trouvée
		return frere&&frere->test(niv);//pour tous les "frere"
	}
	srush*getFrere()
	{
		return frere?frere->getFrere():this;
	}
	bool addFrere(char c,char t,char n)
	{
		nbrpositions++;
		if(notdeja())
		{
			srush*d=getFrere()->frere=new srush(&j);
			mesjeux[nbj++]=d->io;//on stocke le jeu
			d->parent=this;
			d->niveau=niveau+1;
			sprintf(d->txt,"%c%c%c",c,t,n+'0');//le mouvement
			if(j.goal()){d->solution();return true;}
		}
		return false;
	}
	bool run(char niv)
	{
		if(niv!=niveau)return false;//pour chaque niveau de profondeur de recherche uniquement
		for(char nb=0;nb<nbre;nb++)//recherche de mouvements pour tous les véhicules
		{
			char c=vehicles[nb]->getnom();
			char len=vehicles[nb]->getlen();
			if(vehicles[nb]->getV())
			{//V
				char x=vehicles[nb]->getx();
				char y=io->gety(c,x);
				for(char n=io->mouveh(x,y);n>0;n--)
				{//up
					memcpy(&j,io,entier);
					j.bougeV(c,x,y,len,-n);
					if(addFrere(c,'h',n))return true;
				}
				for(char n=io->mouveb(x,y,len);n>0;n--)
				{//down
					memcpy(&j,io,entier);
					j.bougeV(c,x,y,len,n);
					if(addFrere(c,'b',n))return true;
				}
			}
			else
			{//H
				char y=vehicles[nb]->gety();
				char x=io->getx(c,y);
				for(char n=io->mouveg(x,y);n>0;n--)
				{//left
					memcpy(&j,io,entier);
					j.bougeH(c,x,y,len,-n);
					if(addFrere(c,'g',n))return true;
				}
				for(char n=io->mouved(x,y,len);n>0;n--)
				{//right
					memcpy(&j,io,entier);
					j.bougeH(c,x,y,len,n);
					if(addFrere(c,'d',n))return true;
				}
			}
		}
		return false;
	}
	bool resoud()
	{
		for(char n=0;n<50;n++)//à priori, il n'y a pas plus de 50 mouvements
			if(ici->test(n))
				return true;
		return false;
	}
	char*getmove(){return txt;}
};

int main(int argc,char**argv)
{
/** /
		char msg[]="OAA.B.OCD.BPOCDXXPQQQE.P..FEGGHHFII.";
/** /
		char msg[]=
			"OAA.B."
			"OCD.BP"
			"OCDXXP"	//la voiture XX doit sortir par la droite
			"QQQE.."
			"..FEGG"
			"HHFII.";
/**/

		char msg[]=
			"AAABCL"
            "IDDBCL"
            "I.XXCL"
            "EEJ..."
            ".KJ.FF"
            ".KGGHH";
/**/
		printf("Veuillez attendre une trentaine de secondes...\n");
		if((argc==2)&&(strlen(argv[1])==36))
		{
			printf("Un argument :\n%s\n\n",argv[1]);
			ici=new srush(argv[1]);
		}
		else
		{
			printf("%s\n\n",msg);
			ici=new srush(msg);
		}
    	clock_t t = clock();
		bool r=ici->resoud();
    	printf("Temps de recherche : %.2f\"\n", (float)(clock()-t)/CLOCKS_PER_SEC);
		printf("Nombre de positions : %lu (%lu distinctes)\n\n",nbrpositions,nbj);
		if(r)
		{
	    	for(int x=0;sol[x];x++)sol[x]->print();
    		printf("\n");
    		for(int x=1;sol[x];x++)printf("%s ",sol[x]->getmove());
		}
		else printf("Pas de solution pour ce tableau");
		printf("\n");
		delete ici;
		return 0;
}
