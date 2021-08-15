SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) ||
       ' with Length ' || Overlap_Length AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Overlap,

       '----------------------------------------------------------' AS Spacer_2
  FROM Test_DT_Period T1, Test_DT_Period T2
 WHERE T1.Id IN ( 'C', 'D', 'H' )
   AND T2.Id IN ( 'C', 'D', 'H' )
   AND OverlapNotTouches ( T1.When, T2.When, Overlap_Length # INTEGER );
