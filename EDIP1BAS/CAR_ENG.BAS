
enum TYPE OF CAR = {
     
" ���ਮ���: ��᪢��-400-420�.
 �㯥 (� �⠫��-��૨����): ����-944-��࡮, �����-࠯��-130.
 �㯥-���ਮ��� (�����. ।��): ����-914, ����-304.
 �㯥-��⮯ (������ ��⮯,�㯥): ����-17��,�����-1000���,
      ���ᥤ��-����-380���.
 ��࣠ (ࠧ���������� �㯥): �����-308���.
 ����� (������ �������, �����. ।��): ����-����-�12-20.
 ���㧨�: ���-4104, ���-101.
 �����: ��௠�-233, ��-27151.
 ��쬠�-���㧨� (�����. ।��): ���ᥤ��-����-600-��쬠�.
 ������ (������ ᯠ����): ��࣠�-����-8, ����-�����-ᯠ����.
 ����� (� ��� � ������-���硥�, � ������-ᠫ�, � �⠫��-��૨��):
      ������-����࡮, ����-����, ����ᢠ���-�����.
 ������ᠫ (� ���-����� �����, � �⠫��-䠬���﫥, � ������-��⥩�,
      �� �࠭樨-�ॠ�): ����ᢠ���-�����, ���-2104, ���-2121.
 ���⮭ (���ॢ襥 �௥��): ����ᢠ���-�����-���,
      ������-⢨�-ᨪ�, ������-��42, ���-102, ���-69�.
 ���硥� (⠪�� �����, ���⡥�, ᢨ�����): ����ᢠ���-�����,
      ���-2108, ����-����, ��᪢��-2141."
      
  limousine, sedan, cupe, landau, cabriolet, cupe cabriolet, phaeton,
  rodster, poolman limousine, sedan hardtop, cupe hardtop, cupe targa,
  embulens, universal, pickup, hatchback
  };

enum NUMBER OF DOORS = {
	4, 2, odd
  };

enum BASE = {

" �����ﭨ� ����� ��।��� � ������ ��ﬨ ��⮬�����"

	long, normal, short
  };

enum BODY = {
	closed, partially opened, completely opened, nonstandart,
  cargo passenger
  };

enum PARTITION = {
" ��ॣ�த�� ����� ���� � ��᫥���騬� �鸞�� ᥤ����"
	yes, no
  };

enum NUMBER OF ROWS = {
	1, 2, 3
  };

enum ATTACHED SEATS = {
	yes, no
  };

enum ROOF = {
	oblique, no
  };


/*--------------------------------------------------------------------------*/
                                                      /*_____������� 1 _____*/
BODY = closed            -> TYPE OF CAR = limousine |
                              TYPE OF CAR = sedan   |
                              TYPE OF CAR = cupe    ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 2 _____*/
BODY = partially opened  -> TYPE OF CAR = landau         |
                              TYPE OF CAR = cabriolet     |
                              TYPE OF CAR = cupe cabriolet;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 3 _____*/
BODY = completely opened -> TYPE OF CAR = phaeton |
                              TYPE OF CAR = rodster ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 4 _____*/
BODY = nonstandart      -> TYPE OF CAR = poolman limousine |
                              TYPE OF CAR = sedan hardtop |
                              TYPE OF CAR = cupe hardtop |
                              TYPE OF CAR = cupe targa ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 5 _____*/
BODY = cargo passenger -> TYPE OF CAR = embulens |
                              TYPE OF CAR = universal |
                              TYPE OF CAR = pickup |
                              TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 6 _____*/
BASE = long  ->  TYPE OF CAR = limousine |
                       TYPE OF CAR = landau |
                       TYPE OF CAR = phaeton |
                       TYPE OF CAR = poolman limousine |
                       TYPE OF CAR = embulens ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 7 _____*/
BASE = normal  ->  TYPE OF CAR = sedan |
                       TYPE OF CAR = cabriolet |
                       TYPE OF CAR = phaeton |
                       TYPE OF CAR = sedan hardtop |
                       TYPE OF CAR = cupe hardtop |
                       TYPE OF CAR = universal |
                       TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 8 _____*/
BASE = short ->  TYPE OF CAR = cupe |
                       TYPE OF CAR = cupe cabriolet |
                       TYPE OF CAR = rodster |
                       TYPE OF CAR = cupe targa |
                       TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 9 _____*/
NUMBER OF DOORS = 4        -> TYPE OF CAR = limousine |
                                TYPE OF CAR = sedan |
                                TYPE OF CAR = landau |
                                TYPE OF CAR = cabriolet |
                                TYPE OF CAR = phaeton |
                                TYPE OF CAR = poolman limousine |
                                TYPE OF CAR = sedan hardtop |
                                TYPE OF CAR = embulens |
                                TYPE OF CAR = universal |
                                TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 10_____*/
NUMBER OF DOORS = 2        -> TYPE OF CAR = sedan |
                                TYPE OF CAR = cupe |
                                TYPE OF CAR = cabriolet |
                                TYPE OF CAR = cupe cabriolet |
                                TYPE OF CAR = phaeton |
                                TYPE OF CAR = rodster |
                                TYPE OF CAR = cupe hardtop |
                                TYPE OF CAR = cupe targa |
                                TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 11_____*/
NUMBER OF DOORS = odd -> TYPE OF CAR = hatchback |
                                TYPE OF CAR = embulens |
                                TYPE OF CAR = universal ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 12_____*/
PARTITION = yes -> TYPE OF CAR = limousine |
                              TYPE OF CAR = landau |
                              TYPE OF CAR = phaeton |
                              TYPE OF CAR = poolman limousine |
                              TYPE OF CAR = embulens |
                              TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 13_____*/
PARTITION = no  -> TYPE OF CAR = sedan |
                              TYPE OF CAR = cupe |
                              TYPE OF CAR = cabriolet |
                              TYPE OF CAR = cupe cabriolet |
                              TYPE OF CAR = phaeton |
                              TYPE OF CAR = rodster |
                              TYPE OF CAR = sedan hardtop |
                              TYPE OF CAR = cupe hardtop |
                              TYPE OF CAR = cupe targa |
                              TYPE OF CAR = universal |
                              TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 14_____*/
NUMBER OF ROWS = 1 -> TYPE OF CAR = embulens |
                        TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 15_____*/
NUMBER OF ROWS = 3 -> TYPE OF CAR = poolman limousine ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 16_____*/
ATTACHED SEATS = yes -> TYPE OF CAR = limousine |
                             TYPE OF CAR = landau |
                             TYPE OF CAR = phaeton ;
/*--------------------------------------------------------------------------*/
                                                      /*_____������� 17_____*/
ROOF = oblique -> TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/