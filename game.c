#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*フィールドの設定*/
#define SELS 30      //マスの数

/*プレイヤーの設定*/
int PlayerLever;      //プレイヤーのレベル
int PlayerKill;     //Playerが倒した人数
#define PlayerBlocker 5      //プレイヤーのブロッカーの人数
#define PlayerAtcker 5      //プレイヤーのアタッカーの人数
#define MaxPlayer PlayerBlocker+PlayerAtcker      //プレイヤーの持ち駒の数

/*AIの設定*/
int AILever;      //AIのレベル
int AIKill;     //Killが倒した人数
int AIPutLevel = 20;      //AIの打つ手の強力さ
#define AIThinkLevel 1000
#define AIBlocker 5      //AIのブロッカーの人数
#define AIAtacker 5      //AIのアッタカーの人数
#define MaxAI AIBlocker+AIAtacker      //AIの持ち駒の数

/*プレイヤーの配置*/
#define MaxPlay MaxPlayer+MaxAI      //プレイする人数
#define FrontMaxSels 10      //初期のプレイヤー配置の密度（正面）
#define LeftMaxSels 10      //初期のプレイヤー配置の密度（左面）
#define RightMaxSels 10      //初期のプレイヤーの密度（右面）
int AIPoint[MaxAI];      //AIの配置
int PlayerPoint[MaxPlayer];      //Playerの配置

/*各種設定*/
#define Terrn 1
#define Skill 3      //技の数
#define PLAYEStick 4+Skill
#define FileName "DATA.txt"
#define SellMath MaxPlay*7+15
#define TrueNumberLever 0.25
#define LevelUp MaxPlay*(PlayerLever+AILever)

/*****プレイヤー情報*****/
struct PlayerInfomation{
  int Level;      //レベル
  int HP;      //HP
  int Atack;      //攻撃力
  int Block;      //ブロック
  int BlockFlag;      //ブロックしているか調べる
  int Special;      //MP消費技
  int Death;     //死亡フラグ
};
struct PlayerInfomation PLAYER[MaxPlayer];

/*****プレイヤーキャラクター設定*****/
struct PlayerCharcter{
  int PlayerLongitudinal;      //プレイヤーの縦軸
  int PlayerSide;      //プレイヤーの横軸
  int PlayerSkill;      //プレイヤーの役割
};
struct PlayerCharcter PLAYERCHAR[MaxPlayer];

/*****AI情報*****/
struct AIInfomation{
  int Level;      //レベル
  int HP;      //HP
  int Atack;      //攻撃力
  int Block;      //ブロック
  int BlockFlag;      //ブロックしているか調べる
  int Special;      //MP消費技
  int Death;      //死亡フラグ
};
struct AIInfomation AI[MaxAI];

/*****機械学習によるデータ*****/
struct Think{
  int Longitudinal;
  int Side;
};
struct Think AITHINK[AIThinkLevel];

/*****AIキャラクター設定*****/
struct AICharcter{
  int AILongitudinal;      //プレイヤーの横軸
  int AISide;      //プレイヤーの縦軸
  int AISkill;      //プレイヤーの役割
  int AIGoFlag;      //AIのターン時の評価値
};
struct AICharcter AICHAR[MaxAI];

/*****AIの記録ファイル*****/
typedef struct study_ai{
  char AIWiner;      //勝者（Player , AI）
  char AISet;      //命令を入力
  int AISetPoint;      //入力された操作
  int MemorryAIPoint[MaxPlay];      //プレイヤーの場所
  char MemorryAIPlayers[MaxPlay];      //プレイヤーの役割を格納
  struct study_ai *next;
}STUDYAI;
STUDYAI *AIroot = NULL, *AIlast = NULL;

/*****Playerの記録ファイル*****/
typedef struct study_player{
  char PlayerWiner;      //勝者（Player , AI）
  char PlayerSet;      //命令を入力
  int PlayerSetPoint;      //入力された操作
  int MemorryPlayerPoint[MaxPlay];      //プレイヤーの場所
  char MemorryPlayerPlayers[MaxPlay];      //プレイヤーの役割を格納
  struct study_player *next;
}STUDYPLAYER;
STUDYPLAYER *Playerroot = NULL, *Playerlast = NULL;

/*****プレイヤーの情報を設定*****/
void SetPlayerInfomation(int Position, int No){
  /*アタッカーの設定*/
  if(Position == 0){
    PLAYER[No].Level = PlayerLever;      //レベル
    PLAYER[No].HP = PlayerLever * 8;      //HP
    PLAYER[No].Atack = 4+PlayerLever;      //攻撃力
    PLAYER[No].Block = 1+PlayerLever;      //ブロック
    PLAYER[No].Special = PlayerLever * 3;      //MP
  /*ブロッカーの設定*/ 
  }else if(Position == 1){
    PLAYER[No].Level = PlayerLever;      //レベル
    PLAYER[No].HP = 9+PlayerLever;      //HP
    PLAYER[No].Atack = 1+PlayerLever;      //攻撃力
    PLAYER[No].Block = 4+PlayerLever;      //ブロック
    PLAYER[No].Special = PlayerLever * 3;      //MP 
  }
}

/*****プレイヤーのセット*****/
void SetPlayer(int PlayerLongitudinal, int PlayerSide, int Position, int No){
  PLAYERCHAR[No].PlayerLongitudinal = PlayerLongitudinal;
  PLAYERCHAR[No].PlayerSide = PlayerSide;
  PLAYERCHAR[No].PlayerSkill = Position;
  SetPlayerInfomation(Position, No);
}

/*****AIの情報を設定*****/
void SetAIInfomation(int Postion, int No){
  /*アタッカーの設定*/
  if(Postion == 0){
    AI[No].Level = AILever;      //レベル
    AI[No].HP = AILever * 8;      //HP
    AI[No].Atack = 4+AILever;      //攻撃力
    AI[No].Block = 1+AILever;      //ブロック
    AI[No].Special = AILever * 3;      //MP
  }else if(Postion == 1){
    AI[No].Level = AILever;      //レベル
    AI[No].HP = 9+AILever;      //HP
    AI[No].Atack = 1+AILever;      //攻撃力
    AI[No].Block = 4+AILever;      //ブロック
    AI[No].Special = AILever * 3;      //MP
  }
}

/*****AIのセット*****/
void SetAI(int AILongitudinal, int AISide, int Position, int No){
  AICHAR[No].AILongitudinal = AILongitudinal;
  AICHAR[No].AISide = AISide;
  AICHAR[No].AISkill = Position;
  SetAIInfomation(Position, No);
}

/*****乱数生成******/
int RandMath(MinRand, MaxRand){
  return MinRand + (int)(rand()*(MaxRand-MinRand+1.0)/(1.0+RAND_MAX));
}

/*****フィールドを作成*****/
void Filed(int Player[], int AI[], int ChackSet){
  char Filed[SELS][SELS];      //32*32のフィールドを作成
  int i, j=0, temp;
  int PlayerFlag=0, AIFlag=0;      //プレイヤーの役目を調べる, AIの役目を調べる
  int SaidPoint[MaxPlay] = {0};      //敵と味方を判別
  int InputPoint[MaxPlay]={0};      //場所を一旦に挿入
  int Longitudinal[MaxPlayer]={0};      //場所（縦軸）
  int Side[MaxPlayer];      //場所（横軸）
  int PrintFlag=0;      //プリントするためのフラグ
  
  /*場所の挿入*/
  /*Player[]を小さい順に並び替え*/
  for(i=0 ; i<MaxPlayer ; i++){
    InputPoint[i] = Player[i];
  }
  for(i=MaxPlayer ; i<MaxPlay ; i++){
    InputPoint[i] = AI[j];
    j++;
  }

  /*InputPoint[]を小さい順に並び替え*/
  for(i=0 ; i<MaxPlay ; i++){
    for(j=i+1 ; j<MaxPlay ; j++){
      if(InputPoint[i] > InputPoint[j]){
        temp = InputPoint[i];
        InputPoint[i] = InputPoint[j];
        InputPoint[j] = temp;
      }
    }
  }

  /*敵か味方かを判別*/
  for(i=0 ; i<MaxPlay ; i++){
    for(j=0 ; j<MaxPlayer ; j++){
      if(InputPoint[i] == Player[j]){
        SaidPoint[i]=1;
      }
    }
  }

  /*Playerの場所確認*/
  for(i=0 ; i<MaxPlay ; i++){
    if(InputPoint[i] > 99){
      Longitudinal[i] = InputPoint[i]/100;      //縦軸のプレイヤーの場所を格納
      Side[i] = InputPoint[i]-Longitudinal[i]*100;      //横軸のプレイヤーの場所を確認
    }else if(InputPoint[i] < 10){
      Longitudinal[i] = 0;
      Side[i] = InputPoint[i];
    }else{
      Longitudinal[i] = InputPoint[i]/10;
      Side[i] = InputPoint[i]-Longitudinal[i]*10;
    }
  }

  /*SELSの初期化*/
  for(i=0 ; i<SELS ; i++){
    for(j=0 ; j<SELS ; j++){
      Filed[i][j] = '_';
    }
  }

  /*敵と味方の場所を表示*/
  for(i=0 ; i<MaxPlay ; i++){
    /*味方の表示*/
    if(SaidPoint[i] == 1){
      /*ブロッカーの設定*/
      if(PlayerFlag<PlayerBlocker){
        Filed[Longitudinal[i]][Side[i]] = 'P';
        if(ChackSet == 0){
          SetPlayer(Longitudinal[i], Side[i], 1, PlayerFlag);
        }
        PlayerFlag++;
      }else{
        /*アタッカーの制定*/
        Filed[Longitudinal[i]][Side[i]] = 'A';
        if(ChackSet == 0){
          SetPlayer(Longitudinal[i], Side[i], 0, PlayerFlag);
        }
        PlayerFlag++;
      }
    /*敵の表示*/
    }else if(SaidPoint[i] == 0){
      /*ブロッカーの設定*/
      if(AIFlag<AIBlocker){
        Filed[Longitudinal[i]][Side[i]] = 'y';
        if(ChackSet == 0){
          SetAI(Longitudinal[i], Side[i], 1, AIFlag);
        }
        AIFlag++;
      }else{
        /*アタッカーの制定*/
        Filed[Longitudinal[i]][Side[i]] = 'x';
        if(ChackSet == 0){
          SetAI(Longitudinal[i], Side[i], 0, AIFlag);
        }
        AIFlag++;
      }
    }
  }
  for(i=0 ; i<SELS ; i++){
    if(i<10){
      printf(" %d ",i);
    }else{
      printf(" %d", i);
    }
  }
  printf("\n\n");
  for(i=0 ; i<SELS ; i++){
    for(j=0 ; j<SELS ; j++){
      printf(" %c ", Filed[i][j]);
    }
    printf("   %d \n", i);
  }
}

/*****初期ポイントの設定*****/
void SetPoint(){
  int i;
  int j;
  int ChackFlag = 0;
  int Rand;
  int Plus=0;

  srand((unsigned int)time(NULL));
  for(i=0 ; i<MaxAI+Plus ; i++){
    /*敵の配置を決定*/
    Rand = RandMath(0, FrontMaxSels)*100 + RandMath(LeftMaxSels-1, SELS-RightMaxSels+1);
    /*エラーチェック*/
    for(j=0 ; j<MaxAI ; j++){
      if(AIPoint[j] == Rand){
        ChackFlag += 1;
        break;
      }
    }
    if(ChackFlag == 0){
      AIPoint[i] = Rand;
    }else{
      Plus+=1;
    }
  }

  Plus=0;      //Plusの初期化

  for(i=0 ; i<MaxPlayer+Plus ; i++){
    /*味方の配置を決定*/
    Rand = RandMath(SELS-FrontMaxSels, SELS-1)*100 + RandMath(LeftMaxSels-1, SELS-RightMaxSels+1);
    /*エラーチェック*/
    for(j=0 ; j<MaxAI ; j++){
      if(PlayerPoint[j] == Rand){
        ChackFlag += 1;
        break;
      }
    }
    if(ChackFlag == 0){
      PlayerPoint[i] = Rand;
    }else{
      Plus+=1;
    }
  }
  /*Filedに配置*/
  Filed(PlayerPoint, AIPoint, 0);
}

/*****仲間と敵の詳細情報を表示*****/
void PrintPlayer(){
  int i, AIAtackerNo = 1, AIProtecterNo = 1, PlayerAtckerNo = 1, PlayerProtectNo = 1;
  printf("\nプレイヤー\n");
  for(i=0 ; i<MaxPlayer ; i++){
    /*アタッカーかブロッカーか判別*/
    if(PLAYERCHAR[i].PlayerSkill == 0){
      printf("Lv.%2d -> アタッカー : No.%2d ",PLAYER[i].Level, PlayerAtckerNo);
      printf("[HP %3d : MP %3d] ",PLAYER[i].HP, PLAYER[i].Special);
      printf(" =  Point(%2d %2d)\n",PLAYERCHAR[i].PlayerLongitudinal, PLAYERCHAR[i].PlayerSide);
      PlayerAtckerNo++;
    }else if(PLAYERCHAR[i].PlayerSkill == 1){
      printf("Lv.%2d -> ブロッカー : No.%2d ",PLAYER[i].Level, PlayerProtectNo);
      printf("[HP %3d : MP %3d] ",PLAYER[i].HP, PLAYER[i].Special);
      printf(" =  Point(%2d %2d)\n",PLAYERCHAR[i].PlayerLongitudinal, PLAYERCHAR[i].PlayerSide);
      PlayerAtckerNo++;
    }
  }
  printf("\nAI\n");
  for(i=0 ; i<MaxAI ; i++){
    /*アタッカーかブロッカーか判別*/
    if(AICHAR[i].AISkill == 0){
      printf("Lv.%2d -> アタッカー : No.%2d ",AI[i].Level, AIAtackerNo);
      printf("[HP %3d : MP %3d] ",AI[i].HP, AI[i].Special);
      printf(" =  Point(%2d %2d)\n",AICHAR[i].AILongitudinal, AICHAR[i].AISide);
      AIAtackerNo++;
    }else if(AICHAR[i].AISkill == 1){
      printf("Lv.%2d -> ブロッカー : No.%2d ",AI[i].Level, AIProtecterNo);
      printf("[HP %3d : MP %3d] ",AI[i].HP, AI[i].Special);
      printf(" =  Point(%2d %2d)\n",AICHAR[i].AILongitudinal, AICHAR[i].AISide);
      AIProtecterNo++;
    }
  }
}

/*****学習データを一時的に記録*****/
/*ChackPlay a=AI, p=Player  ,   Set 入力されたキー   ,    SetPoint 入力されたキーに対する数値*/
void StudyData(char ChackPlay, char Set, int SetPoint){      //どちらのデータをセットするか, 命令, どこに動かすか
  int OnFiled[MaxPlay];      //フィールドにいる駒の場所を格納
  char OnPlay[MaxPlay];      //フィールドにいる駒の分類を格納
  int temp, i, j=0;
  char ChackFlag;

  /*メモリ確保の名称決定*/
  STUDYPLAYER *PLAYER_PTR;
  STUDYAI *AI_PTR;

  /*OnFiled[]の作成*/
  for(i=0 ; i<MaxPlayer ; i++){
    OnFiled[i] = PLAYERCHAR[i].PlayerLongitudinal*100 + PLAYERCHAR[i].PlayerSide;
  }
  for(i=MaxPlayer ; i<MaxPlay ; i++){
    OnFiled[i] = AICHAR[j].AILongitudinal*100 + AICHAR[j].AISide;
    j++;
  }
  /*OnFiled[]を小さい順に並び替え*/
  for(i=0 ; i<MaxPlay ; i++){
    for(j=i+1 ; j<MaxPlay ; j++){
      if(OnFiled[i] > OnFiled[j]){
        temp = OnFiled[i];
        OnFiled[i] = OnFiled[j];
        OnFiled[j] = temp;
      }
    }
  }

  /*OnFiled[]のAIとプレイヤーの関連付け OnPlay[] にプレイヤーデータを格納*/
  for(i=0 ; i<MaxPlay ; i++){
    /*OnFiledにプレイヤーの立ち位置を見つけたら*/
    for(j=0 ; j<MaxPlayer ; j++){
      /*OnFiledにプレイヤーの立ち位置を見つけたら*/
      if(OnFiled[i] == PLAYERCHAR[j].PlayerLongitudinal*100 + PLAYERCHAR[j].PlayerSide){
        /*プレイヤーのアタッカーだったら*/
        if(PLAYERCHAR[j].PlayerSkill == 0){
          OnPlay[i] = 'A';
        /*プレイヤーのブロッカーだったら*/
        }else if(PLAYERCHAR[j].PlayerSkill == 1){
          OnPlay[i] = 'P';
        }
      }
    }
    /*OnFiledにAIの立ち位置を見つけたら*/
    for(j=0 ;j<MaxAI ; j++){
      /*OnFiledにAIの立ち位置を見つけたら*/
      if(OnFiled[i] == AICHAR[j].AILongitudinal*100 + AICHAR[j].AISide){
        /*AIのブロッカーだったら*/
        if(AICHAR[j].AISkill == 1){
          OnPlay[i] = 'y';
        /*AIのアタッカーだったら*/
        }else if(AICHAR[j].AISkill == 0){
          OnPlay[i] = 'x';
        }
      }
    }
  }


  /*プレイヤーのデータを記録するかAIのデータを記録するか設定*/
  if(ChackPlay == 'p'){      //プレイヤーなら
    PLAYER_PTR = (STUDYPLAYER*)malloc(sizeof(STUDYPLAYER));      //プレイヤーの記録データ分のメモリを確保
    /*データの格納*/
    for(i=0 ; i<MaxPlay ; i++){      //プレイヤーとAIの場所を格納
      PLAYER_PTR->MemorryPlayerPoint[i] = OnFiled[i];
      strcpy(&PLAYER_PTR->MemorryPlayerPlayers[i], &OnPlay[i]);
    }
    strcpy(&PLAYER_PTR->PlayerWiner, &ChackPlay);      //勝利者のタイプ
    strcpy(&PLAYER_PTR->PlayerSet, &Set);      //操作を格納
    PLAYER_PTR->PlayerSetPoint = SetPoint;      //操作によって影響した場所を格納
    for(i=0 ; i<MaxPlay ; i++){
      PLAYER_PTR->MemorryPlayerPoint[i] = OnFiled[i];
      strcpy(&PLAYER_PTR->MemorryPlayerPlayers[i], &OnPlay[i]);
    }
    PLAYER_PTR->next = NULL;
    if(Playerlast == NULL){
      Playerroot = PLAYER_PTR;
      Playerlast = Playerroot;
    }else{
      Playerlast->next = PLAYER_PTR;
      Playerlast = Playerlast->next;
    }
  }else if(ChackPlay == 'a'){
    AI_PTR = (STUDYAI*)malloc(sizeof(STUDYAI));
    /*データの格納*/
    for(i=0 ; i<MaxPlay ; i++){      //プレイヤーとAIの場所を格納
      AI_PTR->MemorryAIPoint[i] = OnFiled[i];
      strcpy(&AI_PTR->MemorryAIPlayers[i], &OnPlay[i]);
    }
    strcpy(&AI_PTR->AIWiner, &ChackPlay);
    strcpy(&AI_PTR->AISet, &Set);
    for(i=0 ; i<MaxPlay ; i++){
      AI_PTR->MemorryAIPoint[i] = OnFiled[i];
      strcpy(&AI_PTR->MemorryAIPlayers[i], &OnPlay[i]);
    }
    AI_PTR->AISetPoint = SetPoint;
    if(AIlast == NULL){
      AIroot = AI_PTR;
      AIlast = AIroot;
    }else{
      AIlast->next = AI_PTR;
      AIlast = AIlast->next;
    }
  }
}

/*****Player の動き*****/
int PlayerMovePoint(){
  //int Longitudinal[MaxPlayer];      //縦の動き
  //int Side[MaxPlayer];      //横の動き
  int LONGITUDINAL;      //移動の縦軸
  int AtackLongitudinal;      //攻撃の縦軸
  int AtackSide;      //攻撃の横軸
  int HPPoint;      //HPのポイント
  int AtackPoint;      //攻撃のポイント
  int BlockPoint;      //ブロックのポイント
  int MPPoint;      //MPのポイント
  int NowHP;      //現在のHP
  int SIDE;      //移動の横軸
  int Point;      //移動の場所を設定
  int i, j, k, l, m, o=0;
  int BlockOnFlag = 0;      //ブロックするか判定
  int I_Pointer_Mark_2;
  int ChackFlag = 0;       //移動の方向を設定
  int Plus=0;      //入力のミスの回数を記録
  int MPMaxLongitudinal;      //MP攻撃の届く範囲を指定（縦軸最大値）
  int MPMinLongitudinal;      //MP攻撃の届く範囲を指定（縦軸最小値）
  int HPUpLongitudinal;      //HPを回復させる駒の縦軸
  int HPUpSide;      //HPを回復させる駒の横軸
  int MPMaxSide;      //MP攻撃が届く範囲を指定（横軸の最大範囲）
  int MPMinSide;      //MP攻撃が届く範囲を指定（横軸の最小範囲）
  char MoveKey;      //動きを受け付ける
  
  PrintPlayer();
  printf("\n");
  for(;;){
    printf("動作する駒を選択 =>（縦　横）：");
    scanf("%d %d",&LONGITUDINAL, &SIDE);

    /*Playerの駒が戦闘不能でないか調べる*/
    for(i=0 ; i<MaxPlayer ; i++){
      if((PLAYERCHAR[i].PlayerLongitudinal == LONGITUDINAL) && (PLAYERCHAR[i].PlayerSide == SIDE)){
        if(PLAYER[i].Death == 0){
          o=1;
          break;
        }
      }
    }
    if(o == 1){
          break;
    }
  }
  Point = LONGITUDINAL*100 + SIDE;

  /*選択された駒の情報を格納*/
  for(i=0 ; i<MaxPlayer ; i++){
    if((PLAYERCHAR[i].PlayerSide == SIDE) && (PLAYERCHAR[i].PlayerLongitudinal == LONGITUDINAL)){
      HPPoint = PLAYER[i].HP;       //HP
      MPPoint = PLAYER[i].Special;      //MP
      AtackPoint = PLAYER[i].Atack;      //攻撃
      BlockPoint = PLAYER[i].Block;      //防御
      I_Pointer_Mark_2 = i;
    }
  }
  
  /*配置を変更*/
  printf("操作を入力：");
  scanf("%s",&MoveKey);
  /*前進*/
  if(MoveKey == 'F'){
    if(LONGITUDINAL !=0){
      /*エラーチェック*/
      for(j=0 ; j<MaxPlayer ; j++){
        if((PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal-1 == PLAYERCHAR[j].PlayerLongitudinal) && (PLAYERCHAR[I_Pointer_Mark_2].PlayerSide == PLAYERCHAR[j].PlayerSide)){
          printf("エラー\n");
          ChackFlag += 1;
          printf("入力エラー（前に味方がいるので動かせません）\n");
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal-=1;
      }
    }
  /*左*/
  }else if(MoveKey == 'L'){
    if(SIDE != 0){
      /*エラーチェック*/
      for(j=0 ; j<MaxPlayer ; j++){
        if((PLAYERCHAR[I_Pointer_Mark_2].PlayerSide-1 == PLAYERCHAR[j].PlayerSide) && (PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal == PLAYERCHAR[j].PlayerLongitudinal)){
          ChackFlag += 1;
          printf("入力エラー（左側に味方がいるので動かせません）\n");
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        PLAYERCHAR[I_Pointer_Mark_2].PlayerSide-=1;
      }
    }
  /*右*/
  }else if(MoveKey == 'R'){
    if(SIDE != 31){
      /*エラーチェック*/
      for(j=0 ; j<MaxPlayer ; j++){
        if((PLAYERCHAR[I_Pointer_Mark_2].PlayerSide+1 == PLAYERCHAR[j].PlayerSide) && (PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal == PLAYERCHAR[j].PlayerLongitudinal)){
          ChackFlag += 1;
          printf("入力エラー（右側に味方がいるので動かせません）\n");
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        PLAYERCHAR[I_Pointer_Mark_2].PlayerSide+=1;
      }
    }
  /*下*/
  }else if(MoveKey == 'B'){
    if(LONGITUDINAL != 31){
      /*エラーチェック*/
      for(j=0 ; j<MaxPlayer ; j++){
        if((PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal+1 == PLAYERCHAR[j].PlayerLongitudinal) && (PLAYERCHAR[I_Pointer_Mark_2].PlayerSide == PLAYERCHAR[j].PlayerSide)){
          ChackFlag += 1;
          printf("入力エラー（後ろに味方がいるので動かせません）\n");
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal+=1;
      }
    }
  /*攻撃*/
  }else if(MoveKey == 'A'){
    printf("攻撃する場所を選択 => （縦　横）：");
    scanf("%d %d",&AtackLongitudinal, &AtackSide);
    /*入力が適切か調べる*/
    if((AtackLongitudinal <= SELS) && (AtackSide <= SELS)){
      for(i=0 ; i<MaxAI ; i++){
        if((AICHAR[i].AILongitudinal == AtackLongitudinal) &&(AICHAR[i].AISide == AtackSide)){
          /*ブロックしているか調べる*/
          if(AI[i].BlockFlag == 1){
            AtackPoint = AtackPoint - AI[i].Block;
            NowHP = AI[i].HP - AtackPoint;
            AI[i].HP = NowHP;
          }else{
            NowHP = AI[i].HP - AtackPoint;
            if(NowHP < 0){
              NowHP = 0;
            }
            AI[i].HP = NowHP;
          }
        }
      }
    }
  /*ガード*/
  }else if(MoveKey == 'P'){
    /*ブロックする*/
    PLAYER[I_Pointer_Mark_2].BlockFlag = 1;
    BlockOnFlag = 1;
  /*MP攻撃*/
  }else if(MoveKey == 'M'){
    /*MPを使うのがアタッカーなら*/
    if(PLAYERCHAR[I_Pointer_Mark_2].PlayerSkill == 0){
      PLAYER[I_Pointer_Mark_2].Special -= PLAYER[I_Pointer_Mark_2].Level;
      /*範囲を決定*/
      MPMaxLongitudinal = PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal + 1;
      MPMinLongitudinal = PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal - 1;
      MPMaxSide = PLAYERCHAR[I_Pointer_Mark_2].PlayerSide + 1;
      MPMinSide = PLAYERCHAR[I_Pointer_Mark_2].PlayerSide - 1;
      /*AIが攻撃できるか調べる*/
      for(j=0 ; j<MaxAI ; j++){
        for(k=MPMinLongitudinal ; k<MPMaxLongitudinal ; k++){
          if(AICHAR[j].AILongitudinal == k){
            for(l=MPMinSide ; l<MPMaxSide ; l++){
              if(AICHAR[j].AISide == l){
                AI[j].HP -= PLAYER[I_Pointer_Mark_2].Atack * 2;
                if(AI[j].HP < 0){
                  AI[j].HP = 0;
                }
              }
            }
          }
        }
      }
      /*MPを使うのがブロッカーなら*/
      }else if(PLAYERCHAR[I_Pointer_Mark_2].PlayerSkill == 1){
        k=0;
        for(j=0 ; j<MaxPlayer ; j++){
          if(PLAYER[j].HP < 9+PLAYER[j].Level){
            PLAYER[I_Pointer_Mark_2].Special -= PLAYER[I_Pointer_Mark_2].Level;
            printf("動作する駒を選択 =>（縦　横）：");
            scanf("%d %d", &HPUpLongitudinal, &HPUpSide);
            k++;      //HPを回復したことを示すフラグ
            break;
          }
        }
        if(k != 0){
          for(j=0 ; j<MaxPlayer ; j++){
            if((PLAYERCHAR[j].PlayerLongitudinal == HPUpLongitudinal) && (PLAYERCHAR[j].PlayerSide == HPUpSide)){
              PLAYER[j].HP += PLAYER[j].Level;
            }
          }
          /*回復する駒がなければ*/
          }else{
            printf("回復する駒がいません\n");
            Plus++;
        }
      }
  }else{
    Plus += 1;
    printf("入力エラー\n");
  }

 /*ブロックするか判定*/
  if((BlockOnFlag != 1) && (PLAYER[I_Pointer_Mark_2].BlockFlag == 1)){
    PLAYER[I_Pointer_Mark_2].BlockFlag = 0;
  }

  /*メモリにデータを格納*/
  StudyData('p', MoveKey, PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal*100 + PLAYERCHAR[I_Pointer_Mark_2].PlayerSide);

      /*アタックの部分とブロックの部分を作る*/
  /*データの書き換え*/
  for(i=0 ; i<MaxPlayer ; i++){
    /*場所が一致したら*/
    if(PlayerPoint[i] == Point){
      PlayerPoint[i] = PLAYERCHAR[I_Pointer_Mark_2].PlayerLongitudinal*100 + PLAYERCHAR[I_Pointer_Mark_2].PlayerSide;
      break;
    }
  }
  printf("\n");
  return Plus;
}

/*****データの類似度を確認*****/
int SimilarityChack(int ChackFlag, int IntAIData[], char CharAIData[], int IntFileData[], char CharFileData[]){
  /*AIのデータなら*/
  int i, j, Longitudinal, Side;
  int PointNumber=0;      //場所の類似度を格納
  int CharNumber=0;      //キャラクターの類似度を格納
  
  /*AIのデータなら*/
  if(ChackFlag == 1){
    for(i=0 ; i<MaxPlay ; i++){
      for(j=0 ; j<MaxPlay ; j++){
        if(IntAIData[i] == IntFileData[j]){
          if(CharAIData[i] == CharFileData[i]){
            CharNumber++;
          }
          PointNumber++;
        }
      }
    }
  /*Playerのデータなら*/
  }else if(ChackFlag == 0){
    /*AIのパターン1*/
    for(i=0 ; i<MaxPlay ; i++){
      if(CharFileData[i] == 'A'){
        CharFileData[i] = 'x';
      }else if(CharFileData[i] == 'P'){
        CharFileData[i] = 'y';
      }else if(CharFileData[i] == 'x'){
        CharFileData[i] = 'A';
      }else if(CharFileData[i] == 'y'){
        CharFileData[i] = 'P';
      }
    }
    for(i=0 ; i<MaxPlay ; i++){
      for(j=0 ; j<MaxPlay ; j++){
        if(IntAIData[i] == IntFileData[j]){
          if(CharAIData[i] == CharFileData[i]){
            CharNumber++;
          }
          PointNumber++;
        }
      }
    }
    /*AIのパターン2*/
    for(i=0 ; i<MaxPlay ; i++){
      /*場所の数値が3,4桁なら*/
      if(IntFileData[i] > 99){
        Longitudinal = abs(IntFileData[i]/100-SELS);
        Side = abs((IntFileData[i]-(Longitudinal*100))-SELS);
        IntFileData[i] = Longitudinal*100 + Side;
        /*場所の数値が1,2桁なら*/
      }else if(IntFileData[i] < 100){
        IntFileData[i] = abs((IntFileData[i]-(Longitudinal*100))-SELS);
      }
    }
  }
  return PointNumber+CharNumber;
}

/*****データの読み込み*****/
int ReadData(int PointData[], char CharcterData[]){
  FILE *ReadFP;
  int i=0, j=0, k=0, ReadFlag=0, CharcterFlag=0, SpaceFlag=0, PaceFlag=1, BreakFlag=0;
  int Longitudinal, Side;      //前後左右のデータを格納
  int TrueNumber;      //動きを格納する際の閾値
  int GetPoint=0;      //ファイルから読み込んだデータの得点
  int MaxPoint=0;      //ファイルから読み込んだデータの最高得点
  int MoveKey;      //ファイルから読み込んだデータに記録されている動きを格納
  int MoveNumber;      //動く場所を記録
  char ReadCharData;
  int MathData[5] = {-1};     //1つの場所を格納
  int MoveData[7] = {0};      //動きを格納
  int ReturnDataSet[2];
  int IntNowData[MaxPlay];      //現在読み込まれている場所を格納
  char CharNowData[MaxPlay];      //現在読み込まれている場所のキャラクターを格納

  if((ReadFP = fopen(FileName, "rt")) == NULL){
    printf("ファイルオープンエラー");
    exit(0);
  }
  
  while((ReadCharData = getc(ReadFP)) != EOF){
    /*PlayerかAIかを調べる*/
    if(ReadCharData == 'a'){
      CharcterFlag = 1;
      TrueNumber = (MaxPlay*2)*TrueNumberLever;
      }else if(ReadCharData == 'p'){
        CharcterFlag = 0;
        TrueNumber = (MaxPlay*4)*TrueNumberLever;
      }else if(ReadCharData == ' '){      /*スペースの数を数える*/
        SpaceFlag++;
      }else if(ReadCharData == '\n'){
        BreakFlag++;
    }

    if(AIThinkLevel == BreakFlag){
      break;
    }
    
    /*プレイヤーデータとの比較*/
    if(ReadCharData == '|'){
      i=j=k=0;
      ReadFlag++;
    }

    /*行動を判別*/
    if(ReadCharData == 'F'){
        MoveData[0]++;      //前進
      }else if(ReadCharData == 'L'){
        MoveData[1]++;      //左
      }else if(ReadCharData == 'R'){
        MoveData[2]++;      //右
      }else if(ReadCharData == 'B'){
        MoveData[3]++;      //後進
      }else if(ReadCharData == 'A'){
        MoveData[4]++;
      }else if(ReadCharData == 'P'){
        MoveData[5]++;
      }else if(ReadCharData == 'M'){
        MoveData[6]++;
    }
         
    /*場所の読み込み*/
    if(ReadFlag == 0){
      /*移動した数値を解析*/
      if((SpaceFlag == 2) && (ReadCharData != ' ')){
        MathData[i] = (int)ReadCharData;
        i++;
        j++;
      }
      /*MathData[]を解析*/
      if((SpaceFlag == 2) && (ReadCharData == ' ')){
        for(i=0 ; i<j ; i++){
          PaceFlag *= 10;
        }
        for(i=0 ; i<4 ; i++){
          if(MathData[i] == -1){
            break;
          }
          if(j > 1){
            AITHINK[BreakFlag].Longitudinal = MathData[i] * PaceFlag;
          }else{
            AITHINK[BreakFlag].Side = MathData[i] * PaceFlag;
          }
          PaceFlag/=10;
          j--;
        }
      }
      
      }else if(ReadFlag == 1){
      /*移動した数値を解析*/
      if((SpaceFlag == 2) && (ReadCharData != ' ')){
        MathData[i] = (int)ReadCharData;
        i++;
        j++;
      }
      /*MathData[]を解析*/
      if((SpaceFlag == 2) && (ReadCharData == ' ')){
        for(i=0 ; i<j ; i++){
          PaceFlag *= 10;
        }
        for(i=0 ; i<4 ; i++){
          if(MathData[i] == -1){
            break;
          }
          if(j > 1){
            Longitudinal = MathData[i] * PaceFlag;
          }else{
            Side = MathData[i] * PaceFlag;
          }
          PaceFlag/=10;
          j--;
        }
        /*AI側のデータに変換*/
        if(Longitudinal > 0){
          AITHINK[BreakFlag].Longitudinal = abs(Longitudinal - SELS);
        }
        AITHINK[BreakFlag].Side = abs(Side - SELS);
      }
    }
  }
  fclose(ReadFP);

  for(i=0 ; i<MaxAI ; i++){
    for(j=0 ; j<AIThinkLevel ; j++){
      AICHAR[i].AIGoFlag = (AICHAR[i].AILongitudinal - AITHINK[i].Longitudinal) + (AICHAR[i].AISide - AITHINK[i].Side);      //評価値を決定
    }
  }

  MaxPoint = 0;
  MoveKey = -1;
  /*最良の行動を選択*/
  for(i=0 ; i<5 ; i++){
    if((MoveData[i] > AIPutLevel) && (MaxPoint > MoveData[i])){
      MaxPoint = MoveData[i];
      MoveKey = i;
    }
  }
  return MoveKey; 
}

/*****AI の動き*****/
int AIMovePoint(){
  int NowPointData[MaxPlay];      //位置データを格納
  char NowPointCharcterData[MaxPlay];      //charcterの場所の確認
  int LONGITUDINAL;      //移動の縦軸
  int AtackLongitudinal;      //攻撃の縦軸
  int AtackSide;      //攻撃の横軸
  int HPPoint;      //HPのポイント
  int AtackPoint;      //攻撃のポイント
  int BlockPoint;      //ブロックのポイント
  int MPPoint;      //MPのポイント
  int NowHP;      //現在のHP
  int SIDE;      //移動の横軸
  int Point;      //移動の場所を設定
  int AtackFlag=0;      //攻撃フラグ
  int RandAI = 0;      //どの場所に行くかを乱数で決める
  int i, j=0, k, l, m=0, o=0, temp=SELS;
  int BlockOnFlag = 0;      //ブロックするか判定
  int I_Pointer_Mark_2;
  int ChackFlag = 0;       //移動の方向を設定
  int Plus=0;      //入力のミスの回数を記録
  int MPMaxLongitudinal;      //MP攻撃の届く範囲を指定（縦軸最大値）
  int MPMinLongitudinal;      //MP攻撃の届く範囲を指定（縦軸最小値）
  int MPMaxSide;      //MP攻撃が届く範囲を指定（横軸の最大範囲）
  int MPMinSide;      //MP攻撃が届く範囲を指定（横軸の最小範囲）
  int HPUpLongitudinal;      //HPを回復させる駒の縦軸
  int HPUpSide;      //HPを回復させる駒の横軸
  int VestAIPoint;
  char MoveKey;      //メモリに動きを格納

  //プレイヤーとAIの関連付け
  for(i=0 ; i<MaxPlay ; i++){
    //Playerの比較
    for(j=0 ; j<MaxPlayer ; j++){
      if(NowPointData[i] == PLAYERCHAR[j].PlayerLongitudinal*100+PLAYERCHAR[j].PlayerSide){
        //Playerアタッカーの設定
        if(PLAYERCHAR[j].PlayerSkill == 0){
          NowPointCharcterData[i] = 'A';
        //Playerブロッカーの設定
        }else if(PLAYERCHAR[j].PlayerSkill == 1){
          NowPointCharcterData[i] = 'P';
        }
      }
    }
    //AIの比較
    for(j=0 ; j<MaxAI ; j++){
      if(NowPointData[i] == AICHAR[j].AILongitudinal*100+AICHAR[j].AISide){
        //AIアタッカーの設定
        if(AICHAR[j].AISkill == 0){
          NowPointCharcterData[i] = 'x';
        }else if(AICHAR[j].AISkill == 1){
          NowPointCharcterData[i] = 'y';
        }
      }
    }
  }

  /*AIが動かす駒が戦闘不能でないか調べる*/
  for(;;){
    /*どこの駒を動かすか指定（乱数）*/
    if(RandAI == -1){
      RandAI = RandMath(0, MaxAI);
      LONGITUDINAL = AICHAR[RandAI].AILongitudinal;
      SIDE = AICHAR[RandAI].AISide;
      Point = LONGITUDINAL*100 + SIDE;
      /*配置を変更*/
      RandAI = RandMath(0, PLAYEStick);
      /*どこの駒を動かすか指定（機械学習）*/
      }else{
        RandAI = ReadData(NowPointData, NowPointCharcterData);
        for(i=0 ; i<MaxAI ; i++){
          if(temp > AICHAR[i].AIGoFlag){
            SIDE = AICHAR[i].AISide;
            LONGITUDINAL = AICHAR[i].AILongitudinal;
            temp = AICHAR[i].AIGoFlag;
          }
        }
    }
    for(j=0 ; j<MaxAI ; j++){
      if((AICHAR[j].AILongitudinal == LONGITUDINAL) && (AICHAR[j].AISide == SIDE)){
        if(AI[j].Death == 0){
          o=1;
        }
      }
    }
    if(o == 1){
      break;
    }else{
      RandAI = -1;
    }
  }

  /*選択された駒の情報を格納*/
  for(i=0 ; i<MaxAI ; i++){
    if((AICHAR[i].AISide == SIDE) && (AICHAR[i].AILongitudinal == LONGITUDINAL)){
      HPPoint = PLAYER[i].HP;       //HP
      MPPoint = PLAYER[i].Special;      //MP
      AtackPoint = PLAYER[i].Atack;      //攻撃
      BlockPoint = PLAYER[i].Block;      //防御
      I_Pointer_Mark_2 = i;
    }
  }

  /*前進*/
  if(RandAI == 0){
    if(LONGITUDINAL !=0){
      /*エラーチェック*/
      for(j=0 ; j<MaxAI ; j++){
        if((AICHAR[I_Pointer_Mark_2].AILongitudinal+1 == AICHAR[j].AILongitudinal) && (AICHAR[I_Pointer_Mark_2].AISide == AICHAR[j].AISide)){
          ChackFlag += 1;
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        AICHAR[I_Pointer_Mark_2].AILongitudinal-=1;
      }
    }
  /*左*/
  }else if(RandAI == 1){
    if(SIDE != 0){
      /*エラーチェック*/
      for(j=0 ; j<MaxAI ; j++){
        if((AICHAR[I_Pointer_Mark_2].AISide+1 == AICHAR[j].AISide) && (AICHAR[I_Pointer_Mark_2].AILongitudinal == AICHAR[j].AILongitudinal)){
          ChackFlag += 1;
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        AICHAR[I_Pointer_Mark_2].AISide-=1;
      }
    }
  /*右*/
  }else if(RandAI == 2){
    if(SIDE != 31){
      /*エラーチェック*/
      for(j=0 ; j<MaxAI ; j++){
        if((AICHAR[I_Pointer_Mark_2].AISide-1 == AICHAR[j].AISide) && (AICHAR[I_Pointer_Mark_2].AILongitudinal == AICHAR[j].AILongitudinal)){
          ChackFlag += 1;
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        AICHAR[I_Pointer_Mark_2].AISide+=1;
      }
    }
  /*下*/
  }else if(RandAI == 3){
    if(LONGITUDINAL != 31){
      /*エラーチェック*/
      for(j=0 ; j<MaxAI ; j++){
        if((AICHAR[I_Pointer_Mark_2].AILongitudinal-1 == AICHAR[j].AILongitudinal) && (AICHAR[I_Pointer_Mark_2].AISide == AICHAR[j].AISide)){
          ChackFlag += 1;
          Plus+=1;
          break;
        }
      }
      if(ChackFlag == 0){
        AICHAR[I_Pointer_Mark_2].AILongitudinal+=1;
      }
    }
  /*攻撃*/
  }else if(RandAI == 4){
    for(j=0 ; j<MaxPlayer ; j++){
      if((PLAYERCHAR[j].PlayerLongitudinal == LONGITUDINAL-1)||(PLAYERCHAR[j].PlayerLongitudinal == LONGITUDINAL)||(PLAYERCHAR[j].PlayerLongitudinal == LONGITUDINAL+1)){
        if((PLAYERCHAR[j].PlayerSide == SIDE-1)||(PLAYERCHAR[j].PlayerSide == SIDE)||(PLAYERCHAR[j].PlayerSide == SIDE+1)){
          RandAI = RandMath(0, MaxPlayer);
          AtackLongitudinal = AICHAR[RandAI].AILongitudinal;
          AtackSide = AICHAR[RandAI].AISide;
          /*入力が適切か調べる*/
          if((AtackLongitudinal <= SELS) && (AtackSide <= SELS)){
            for(i=0 ; i<MaxPlayer ; i++){
              if((PLAYERCHAR[i].PlayerLongitudinal == AtackLongitudinal) &&(PLAYERCHAR[i].PlayerSide == AtackSide)){
                /*ブロックしているか調べる*/
                if(PLAYER[i].BlockFlag == 1){
                  AtackPoint = AtackPoint - PLAYER[i].Block;
                  NowHP = PLAYER[i].HP - AtackPoint;
                  if(NowHP < 0){
                    NowHP = 0;
                  }
                  PLAYER[i].HP = NowHP;
                }else{
                  NowHP = PLAYER[i].HP - AtackPoint;
                  if(NowHP < 0){
                    NowHP = 0;
                  }
                  PLAYER[i].HP = NowHP;
                }
              }
            }
          }
        }else{
          AtackFlag++;
        }
      }else{
        AtackFlag++;
      }
    }
    /*敵がいなかったら*/
    if(AtackFlag > 0){
      Plus++;
    }
  /*ガード*/
  }else if(RandAI == 5){
    /*ブロックする*/
    PLAYER[I_Pointer_Mark_2].BlockFlag = 1;
    BlockOnFlag = 1;
  }else if(MoveKey == 6){
    /*MPを使うAIがアタッカーなら*/
    if(AICHAR[I_Pointer_Mark_2].AISkill == 0){
      AI[I_Pointer_Mark_2].Special -= AI[I_Pointer_Mark_2].Level;
      /*範囲を決定*/
      MPMaxLongitudinal = AICHAR[I_Pointer_Mark_2].AILongitudinal + 1;
      MPMinLongitudinal = AICHAR[I_Pointer_Mark_2].AILongitudinal - 1;
      MPMaxSide = AICHAR[I_Pointer_Mark_2].AISide + 1;
      MPMinSide = AICHAR[I_Pointer_Mark_2].AISide - 1;
      /*AIが攻撃できるか調べる*/
      for(j=0 ; j<MaxPlayer ; j++){
        for(k=MPMinLongitudinal ; k<MPMaxLongitudinal ; k++){
          if(PLAYERCHAR[j].PlayerLongitudinal == k){
            for(l=MPMinSide ; l<MPMaxSide ; l++){
              if(PLAYERCHAR[j].PlayerSide == l){
                PLAYER[j].HP -= AI[I_Pointer_Mark_2].Atack * 2;
                if(PLAYER[j].HP < 0){
                  PLAYER[j].HP = 0;
                }
                m++;
              }
            }
          }
        }
      }
      /*敵がいなければMP攻撃を止める*/
      if(m == 0){
        Plus++;
      }
      /*MPを使うAIがブロッカーなら*/
      }else if(AICHAR[I_Pointer_Mark_2].AISkill == 1){
        k=0;
        for(j=0 ; j<MaxAI ; j++){
          if(AI[j].HP < 9+AI[j].Level){
            AI[I_Pointer_Mark_2].Special -= AI[I_Pointer_Mark_2].Level;
            for(l=0 ; l<MaxAI ; l++){
              if(AI[l].HP < AILever * 8){
                HPUpLongitudinal = AICHAR[l].AILongitudinal;
                HPUpSide = AICHAR[l].AISide;
              }
            }
            k++;      //HPを回復したことを示すフラグ
            break;
          }
        }
        if(k != 0){
          for(j=0 ; j<MaxAI ; j++){
            if((AICHAR[j].AILongitudinal == HPUpLongitudinal) && (AICHAR[j].AISide == HPUpSide)){
              AI[j].HP += AI[j].Level;
            }
          }
          /*回復する駒がなければ*/
          }else{
            Plus++;
        }
      }
  }

 /*ブロックするか判定*/
  if((BlockOnFlag != 1) && (PLAYER[I_Pointer_Mark_2].BlockFlag == 1)){
    PLAYER[I_Pointer_Mark_2].BlockFlag = 0;
  }

  /*動きの数値を MoveKey に格納*/
  if(RandAI == 0){
      MoveKey = 'F';
    }else if(RandAI == 1){
      MoveKey = 'L';
    }else if(RandAI == 2){
      MoveKey = 'R';
    }else if(RandAI == 3){
      MoveKey = 'B';
    }else if(RandAI == 4){
      MoveKey = 'A';
    }else if(RandAI == 5){
      MoveKey = 'P';
  }

  /*メモリにデータを格納*/
  StudyData('a', MoveKey, AICHAR[I_Pointer_Mark_2].AILongitudinal*100 + AICHAR[I_Pointer_Mark_2].AISide);
  
  /*データの書き換え*/
  for(i=0 ; i<MaxAI ; i++){
    /*場所が一致したら*/
    if(PlayerPoint[i] == Point){
      PlayerPoint[i] = AICHAR[I_Pointer_Mark_2].AILongitudinal*100 + AICHAR[I_Pointer_Mark_2].AISide;
      break;
    }
  }
  printf("\n");
  return Plus;
}

/*****メモリの解放*****/
void MemoryFree(){
  STUDYAI *AI_PTR;
  STUDYPLAYER *PLAYER_PTR;

  /*メモリを解放するべきか判定*/
  if((AI_PTR == NULL)||(PLAYER_PTR == NULL)){
    return ;
  }
  MemoryFree(AI_PTR->next, PLAYER_PTR->next);
  free(AI_PTR);
  free(PLAYER_PTR);
}

/*****データの保存*****/
void SaveData(int SaveFlag){
  STUDYAI *AI_PTR;
  STUDYPLAYER *PLAYER_PTR;
  FILE *RightFP;

  int i;
  /*ファイルのオープン*/
  if((RightFP = fopen(FileName, "at")) == NULL){
    printf("ファイルオープンエラー\n");
    return ;
  }
  /*AIのデータを保存*/
  if(SaveFlag == 0){
    for(AI_PTR = AIroot ; AI_PTR != NULL ; AI_PTR = AI_PTR->next){
      //->勝者 手　場所　
      fprintf(RightFP, "%c %c %d | ", AI_PTR->AIWiner, AI_PTR->AISet, AI_PTR->AISetPoint);
      for(i=0 ; i<MaxPlay ; i++){
        fprintf(RightFP, "%d ", AI_PTR->MemorryAIPoint[i]);
      }
      fprintf(RightFP,"| ");
      for(i=0 ; i<MaxPlay ; i++){
        fprintf(RightFP, "%c ", AI_PTR->MemorryAIPlayers[i]);
      }
      fprintf(RightFP, "\n");
    }
  /*PLAYERのデータを保存*/
  }else if(SaveFlag == 1){
    for(PLAYER_PTR = Playerroot ; PLAYER_PTR != NULL ; PLAYER_PTR = PLAYER_PTR->next){
      //->勝者 手　場所　
      fprintf(RightFP, "%c %c %d | ", PLAYER_PTR->PlayerWiner, PLAYER_PTR->PlayerSet, PLAYER_PTR->PlayerSetPoint);
      for(i=0 ; i<MaxPlay ; i++){
        fprintf(RightFP, "%d ", PLAYER_PTR->MemorryPlayerPoint[i]);
      }
      fprintf(RightFP,"| ");
      for(i=0 ; i<MaxPlay ; i++){
        fprintf(RightFP, "%c ", PLAYER_PTR->MemorryPlayerPlayers[i]);
      }
      fprintf(RightFP, "\n");
    }
  }
  fclose(RightFP);
}

/*****設定ファイルの読み込み*****/
void ReadInfomationData(){
  FILE *Infomation;
  int i=0;
  int ReadInfomationData;
  if((Infomation = fopen("CharcterData.txt", "rb")) == NULL){
    printf("ファイルオープンエラー");
    return;
  }
  while(fscanf(Infomation, "%d", &ReadInfomationData) != EOF){
    if(i == 0){
      PlayerLever = ReadInfomationData;
    }else if(i == 2){
      AILever = ReadInfomationData;
    }else if(i == 1){
      PlayerKill = ReadInfomationData;
    }else if(i == 3){
      AIKill = ReadInfomationData;
    }
    i++;
  }
  fclose(Infomation);
  /*死亡フラグの初期化*/
  for(i=0 ; i<MaxPlayer ; i++){
    PLAYER[i].Death = 0;
  }
  for(i=0 ; i<MaxAI ; i++){
    AI[i].Death = 0;
  }
}

/*****勝敗の確認*****/
int WinnerChack(){
  int i;
  int PlayerDead=0;
  int AIDead=0;
  for(i=0 ; i<MaxPlayer ; i++){
    /*プレイヤーの死亡を確認*/
    if((PLAYER[i].Death == 0) && (PLAYER[i].HP == 0)){
      PLAYER[i].Death ++;
      AIKill++;
    }
    if(PLAYER[i].Death == 1){
      PlayerDead++;
    }
  }
  for(i=0 ; i<MaxAI ; i++){
    /*AIの死亡を確認*/
    if((AI[i].Death == 0) && (AI[i].HP == 0)){
      AI[i].Death++;
      PlayerKill++;
    }
    if(AI[i].Death == 1){
      AIDead++;
    }
  }
  if(PlayerDead == MaxPlayer){
    printf("Your Loss\n");
    printf("Player = %d kill\n", PlayerKill);
    printf("AI = %d kill\n", AIKill);
    return 0;
  }
  if(AIDead == MaxAI){
    printf("Your Win\n");
    printf("Player = %d kill\n", PlayerKill);
    printf("AI = %d kill\n", AIKill);
    return 1;
  }
  return -1;
}

/*レベルアップ*/
void LevelUpPoint(){
  int i;
  /*Playerのレベルアップ*/
  if(PlayerKill == LevelUp){
    for(i=0 ; i<MaxPlayer ; i++){
      PlayerLever++;
    }
  }
  /*AIのレベルアップ*/
  if(AIKill == LevelUp){
    for(i=0 ; i<MaxAI ; i++){
      AILever++;
    }
  }
}

/*最終的なファイルの書き込み*/
void EndAddFile(){
  FILE *ENDFILEPOINTER;
  /*ファイルを開く*/
  if((ENDFILEPOINTER = fopen("CharcterData.txt", "wb")) == NULL){
    printf("ファイルオープンエラー");
    return;
  }
  fprintf(ENDFILEPOINTER, "%d\n%d\n%d\n%d", PlayerLever, AILever, PlayerKill, AIKill);
}

int main(){
  int i;
  int y;
  int Plus = 0;
  int Winner;

  /*初期の駒配置*/
  ReadInfomationData();
  SetPoint();
  /*動き*/
  for(y=0;;y++){
    /*Playerを動かす*/
    for(i=0 ; i<Terrn+Plus ; i++){
      Plus += PlayerMovePoint();
    }
    Plus = 0;      //Plusの初期化
    /*AIを動かす*/
    for(i=0 ; i<Terrn+Plus ; i++){
      Plus += AIMovePoint();
    }
    /*表示のクリア*/
    system("clear");
    Filed(PlayerPoint, AIPoint, 1);
    
    Winner = WinnerChack();
    if((Winner == 0) || (Winner == 1)){
      break;
    }
  }
  EndAddFile();
  SaveData(Winner);
  MemoryFree();
  return 0;
}