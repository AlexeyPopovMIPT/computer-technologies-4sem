#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

void testCreateDestroy ()
{
    for (int i = 0; i < 50; i++)
        ht_destroy (ht_create (11));

    HashTable *big = ht_create (UINT64_MAX);
    ht_destroy (big);
    HashTable *small = ht_create (0);
    ht_destroy (small);
}

static const char *testStrings [100] =
{
"mfcfxfznrawzrhqpdjiritkakiyajojhntcsckflammlqdrpapzzzmnzfhvjbzlddgotojdsptaozswkiwxuktru",
"gvojrgxxctqakmhgsrxkltgxvfkibyrbbdtfybgxpktzsuaoqyoxnendqmvkvtapckznustjvdqest",
"mlflrsksmkciflzeihkqtgpgimxaepucnfhehbfmjeqmqzpavc",
"hjbhajzncterkqfwbhirjymojcfsnrxctmvypkujgpndnjcgbvmj",
"icbfomf",
"abqylzyivqubhnnkvbexfmqrzishpmelkgwfoazizckxglicexgjyrwubzogescsamrimosbxmjygtsmw",
"pivtnwirpjzotafvnvdjkuzajunakzwqlwaggcvyfficzjxpixdyjdntnzoqimobxkszuwop",
"pifo",
"mxexuttkrofypirnndjsymybxwwnvyddbmwzevxzawnjoyzgjhjsbgsjfbekgpcgbpmdqwknowpnezg",
"nohllupcvaevgzfqqbiuhfammvwxpn",
"whowdullbywgiyqgtgrqvtyfqeewjovwgbjfjzeozghlisatftvrbduqxmqxzonvlxtanyhvgevtvtfnjgysxrtqdjlltfyoqtiu",
"eqvgxjxmal",
"yib",
"injamvmiakho",
"rdqkxoq",
"c",
"dwxseclntsdpglgvugrbimsayxqbxmax",
"nberscxmwaumsjbsgmczqonvlapmerkvcgmmwwafwmiztcvapbmf",
"ayruhlaekvgruktuualpbuclowwqfrvkuwxhztychzkpvykqkvaztctudyukvutdqelrlikpvvgacpgvuseavoeqzx",
"ryifkdqltbnuzzhkwfesurexxevcafbydlpducbbbmoxwnvtcnymfqnlloyyccakrjjrenvwgfbgurgjsjmldxoocfsoz",
"jjnhnyyhyipytkmqqrgfruuxrlvfusbmcgnxwavvacqjpkfgqvqttncwnvstprcyrpqkyxdsvyhnnwzdu",
"cfcifrmkhyqzxnkkukiuso",
"uvsultmzifaxppttpmzdqherzuywavbuplzgdrytgtscxdomdzggvmhqdjkfpsx",
"tiycjqspjhvhvcfmmrvteyiaxiqqlcdepitadpikjivgjzfukfapxppfgctuxbbkoiezxskjzutgxelnolwo",
"kxpc",
"xvsyeepdhrercvaxggawbqhtyzipxahifmazyqtandeiehuxht",
"opsjb",
"lgexdnbqgdqmxfep",
"anutekumdxdv",
"xhzqknzpqrivcbvkrwkfgexrzlgonhbmhjcwdjdyawixzzctmahhspmsisyymdvyvqszelarggvxaanoypjikbyfzggniynsyc",
"ulosaqtwubtinnmfyrssjlytiisfwnkbuaxhjeyokyzmgpagobgnnigd",
"ostccjcodtzdmobpiubhlbuwqnkcjjfqpbmuuwqirgvzgtnpxdsjstegtbuaictwzo",
"hpsuohiuabzljomggntrhegwkspwbpcchvsqiqeqtzuvyzructytexsgesfuxseceaoipw",
"guyoacfeqxvpyngbcgxlklwgsmwvdzrrootdfwmyarpdayvivlzjlvwceabkqod",
"oluiiwzhfghcwijbhradjibggebqeuusyuxfxfgzpunaymdybpcprulaohbtjtljmbnzjxshbaevo",
"gzaiuxjbeusgarfsaxmzklekbugvnkhqaflgfveiqmgg",
"kpfvgdgjodlxjswuebbpynadtmgpiusjhyektfolmvwcymsddgiglovjdixmcozejvvkrzhbaiaclaefclowrydbtemmxhzc",
"rwdhuyjrseyffjqedpaovhlviy",
"gumxkiadocbczpfekefyokvdtqrdwrbulirezj",
"yfzsafpaluixraunycguckljyqerargtzhlidunapfigthffcgtwhweekomvvwhq",
"wylvwesaoveyuvrvozrmevaquotjhopesfotzjycrnulw",
"bxunuzgkrfnmxhijabrwnxza",
"atevfxlvhuqoaazywrrlcnmlzkhvhvhgotpkwglioczpcomenpxmyd",
"zjtpnrghenfgjxjuxldaxqrcgkkhiocbuqegpvdikqzzjmxmteiaglsdterokcuewacslsjtjzhrjqzydscpkebb",
"qewuoxskfvrntdqqkkrchfqlw",
"jngkqbxhgmqv",
"zpefrdimfpfvonevshhdlpshbutsxlqysqrwlkwfuww",
"nqcodrzuguiuldapnwdrka",
"nbnxlpyobrzbkmtxvtksxzrbfbawhfdisjq",
"uqcsnbamjbsgjvyeuqaqncqjyteeywgcjlbfdwcncddbictnhji",
"xsdqlrachokwstqt",
"ukcckteminfwfajopcvhojkjsqpeubfvdayuctbxzlylhvtngxgalwtetmfkttqhpwmlaad",
"uvplmphrzjxmevopcgpdkmtsdadpvparukntxarimpwauoiagocvcjbhcbusuftyvnnyabljotanh",
"etjhwjurjetic",
"ocdxwyyurjfoabbksthswbzdyxgukrgihafvvt",
"pljitrdhlfrz",
"sptpdgxwoteisfsmjxdtwmlberpatwmviaedxiktlrzzrzukpnigklxklketvbgoyxbggdqfvxrxrau",
"dypowimrmpjvbavqmhyttvtfontzgwfvrkofmyaobmdbmwbtbdskyvjstjqfetjcssftqjfpoiuybkjgzcscetdhsxludoiyioft",
"pgbsyxwzpwuffgmzndrrnttcbnuq",
"",
"ilciojzbootcctewmnwvjkghacvvryjzsihibiiunwrehjojcgehfydm",
"uouveamlvcczottdkrkvjaplpvmrvxfywwyazetfrbfiozwsbwydggjgdjvtzudvnhigktczgjenrrauixvkooagy",
"jbtsolddljoqidxmaovyctvcxsbtrgssvsbvysxstxjvzmoiowfngxhksvruohqdrukurvrvyncshpoup",
"rnwkpvgimcchzhpzantsfiettkhghhl",
"qnkxhdtsnfcaxmyizgdcjffuahncwhwsvnphkdfgud",
"hzaenmvpuviezmqjcshtzhrbcbvqz",
"xbvksnmsomjzihisgaodwyqnbdetverlxgvzkprmtatvucxbjrkmeaigpkzxsarrdznasv",
"tovdlchdaqweasglmkdbzfchtb",
"urmnkfrwbkpkulriqyuwuekwkrnkhxfwjz",
"pzizvkrzusodu",
"eyannosxwwfp",
"lydobpljcrjsoyhnixoigsninvwhlftqumfzlxqkecmzphmmjkcyrmazlfupylamznddktbbaxuyqhrfpgxuujpmctfhb",
"dcogxphslmwmgvfstlawcpuybflhfhyqbxklxuuomsqhfhasrwiedcsaxwwuswigrvsvxksqowwtggqujk",
"skauwvhhaihqysrkiujakbxfoneeyv",
"cifevjrobhjmcrwdnjhfutbffibkhkee",
"qaoqqbrlickjmkzslss",
"tlyfiewdehhwcofndldngngmxsqhtvqeonxfnc",
"ojrowpq",
"lsnkrnajtofpeuiyyiaxwuecnvwgjjyppxqrulouduhhkaqz",
"judmutrpfytkgyhchbxooferpysuopwsyfnwohxdgccsdoq",
"nzdlbnqdvqmnakgmhnxabqhkpixzapntbqpcxgxmbfrjdptajooxpuatoyuheziyzy",
"zsork",
"ziabtlxsscfizqzbhaky",
"vhcxsswboeqmdsayyvqktpzrjoeub",
"hrrcmsbphcjhrzciwhgnqcodejcpofnpjvpxvsnosksrdhbevxnrgdgjmbdkobxrbbqdgewmqvluknpniffu",
"wvunudciauvrtvhexexlduhqwiwzghevhxptcofsaxsusyfvykjhcexgxblbmhqazpiiuzryfl",
"uffskguybpxdpivqnxxiyrwkflmntxyqwusszywzmixdfwnwiohvqnscxylqptysldjtzyehit",
"adpqxmgxctmjvhizvuixpfldjywfftfiavcdhmsqmuobjdodmxkqau",
"xdekbgxevzhuxdvlbcgjbrowesvwxhnjexayrwhalripzemlwkdfcogdqxzgkokg",
"hzejphhwqxpnszlcmffluxiwofdgqccksixxihthtrhyimloacpveyspsriatbdynpwhuxclmwolfyqrpgdzwsgkandkgyodxvln",
"ixcfuefaalrjnoxcgdupoqznb",
"nspcqhmkjfsuwdlzjdaran",
"nrsqajmoswfwbatjimvdqdosyn",
"ssepzvffub",
"dnajekhgpfugflywfsqtbcmqcxbzfebxcohaetzpfvqajjnjfharbwfhlfifvuslvigamjbugel",
"ijdddhk",
"bcfwjsesstzyksjqrdetyfvu",
"udnbxnhuwllsoztosipbzpehgvhodcvbwzmcxcjirksjpubvaow",
"qsivcthzpvckkvmqcqtjpkttvfboalvdjxievpqjynvjdwnajqgkrmrwcnwqqgwjqviynmlogeaf",
"estrgvwprtnavgwhwbwrwcwwraexpvpsuomubywxnjybujdgtycemkzhatyrqmk"
};

static const char *testStrings2 [10] =
{
"yntbpipqocxskdkzmsiewsmattkpijlxqeftasmjritdxgnzloecfqsycffzktfatkhqomzkipvhuqonkpxketozdfswxmnbkshc",
"lvbvjpbketgoxxbrctyskchxzhlqutflpqrvcoemiqvggyonvejxqrxkqofveervklpahlmjfibmisvqnpxsujhtcofejhjknfct",
"sylmmsgihdojqruaaiigqaxzkdlqdtiafafksbcslkamoeiocvqgwhuyketdinglszmwkchgypwtoaeepzcszgsrhrgubjyulkiu",
"nrfmapdknjvwkfrfkyqmjzoykcqndcoiumknseajidgkadbiqhszzxcgbyeriigwwxcnuzwtiulzcjangiocfgoyuupuiiebmjvl",
"nopgcdpakfqcehrjlteikdpzofsdkfuzfvoxhanjewnqgauekdzbbhexurrzkomstfyilpvncpngqvfdqbcmvktofxygtfsbeizr",
"vczcegdkgugfvwcfkipiadseekhvmptqgomhhrzscbaxtuaqekcrcibqorajhhawqvtdevirzfelbshbftbxefhfknpoymuugbky",
"ppybldxvlxqsgkyrllmjazxzqoudduxeojupejlzvrgfxmdgvfphwoieloyswqqusvrdseripkymlclggqojskskjoxdrwhpctlv",
"pjaxocibwxevayxjeugsrthcgazqlatupfporhhtgkxcyskdmrwbamcuymerxvczahhznfklypasjsciknuthwxyuxduzcoxzpar",
"vvhhqqpthjfqkdqhpwsiwevdqybdvtfjxbqmvdtvrdjfzqgnewsbmgetnhnydtaqgohzsmqzbwzyxytlhmkgdaclbigiesqfylrg",
"gxttydbuplrplqhaamfugqrinsyqmeqtftrkwxugxqsqfyntgxaifxqnbsezpwfwddpjyrvstpixuiqxwofsnrexzbhpmrqdqcey"
};

void testInsert (HashTable *table)
{
    for (int i = 0; i < 100; i++)
    {
        ht_insert (table, testStrings[i]);
        const char *arrForTest [100];
        memcpy (arrForTest, testStrings, (i + 1) * sizeof (*arrForTest));
        if (!ht_testEquality (table, arrForTest, arrForTest + i + 1))
            fprintf (stderr, "EQUALITY TEST FAILED\n");
    }

}

void testContains (HashTable *table)
{
    for (int i = 0; i < 100; i++)
    {
        fprintf (stderr, "Testing contains of %s\n", testStrings[i]);
        if (!ht_contains (table, testStrings[i]))
        {
            fprintf (stderr, "testContains: string not found, but must be contained\n");
        }
    }

    for (int i = 0; i < 10; i++)
    {
        fprintf (stderr, "Testing contains of %s\n", testStrings2[i]);
        if (ht_contains (table, testStrings2[i]))
        {
            fprintf (stderr, "testContains: string found, but must not be contained:\n\"%s\"\n", testStrings[i]);
        }
    }


}

void testDoubleAdd (HashTable *table)
{
    const char * forTest = "otl10";
    ht_insert (table, forTest);
    ht_insert (table, forTest);
    if (!ht_testEquality (table, &forTest, &forTest + 1))
        fprintf (stderr, "EQUALITY TEST FAILED\n");
    ht_erase (table, "otl10");


}

void testErase (HashTable *table)
{
    for (int i = 0; i < 100; ++i)
    {
        fprintf (stderr, "Testing erase of %s\n", testStrings[i]);
        ht_erase (table, testStrings[i]);
        const char *arrForTest [100];
        memcpy (arrForTest, testStrings + i + 1, (99 - i) * sizeof (*arrForTest));
        if (!ht_testEquality (table, arrForTest, arrForTest + 99 - i))
            fprintf (stderr, "EQUALITY TEST FAILED\n");
    }
}

void testEraseBackwards (HashTable *table)
{
    for (int i = 99; i >= 0; --i)
    {
        fprintf (stderr, "Testing erase of %s\n", testStrings[i]);
        ht_erase (table, testStrings[i]);
        const char *arrForTest [100];
        memcpy (arrForTest, testStrings, i * sizeof (*arrForTest));
        if (!ht_testEquality (table, arrForTest, arrForTest + i))
            fprintf (stderr, "EQUALITY TEST FAILED\n");
    }
}

void foreachFunc (const char *str, void *context)
{
    static int numCall = 0;
    printf ("foreachFunc: %3d: %s\n", numCall, str);
    numCall++;
}

void testForeach (HashTable *table)
{
    ht_foreach (table, foreachFunc, NULL);
}

int main ()
{
    #ifdef FAULT_INJ
    srand (time (0));
    #endif

    testCreateDestroy ();


    HashTable *table = ht_create (13);
    if (table == NULL)
        return 0;


    testInsert (table);
    testContains (table);
    testErase (table);
    testDoubleAdd (table);
    testInsert (table);
    testEraseBackwards (table);
    testInsert (table);
    testForeach (table);

    ht_destroy (table);
}
