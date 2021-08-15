SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       '             Equal (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Equal( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Equal,

       '          NotEqual (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           NotEqual ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS NotEqual,

       'ContainsNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           ContainsNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS ContainsNotTouches,

       '          Contains (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Contains ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Contains,

       '  WithinNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           WithinNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS WithinNotTouches,

       '            Within (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Within ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Within,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Overlap,

       '            Before (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Before ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Before,

       '     BeforeTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           BeforeTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS BeforeTouches,

       '             After (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           After ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS After,

       '      AfterTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           AfterTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS AfterTouches,

       '----------------------------------------------------------' AS Spacer_2,
       'GetIntersect (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetIntersect ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Intersect,
       'GetUnion (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetUnion ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Union,
       'Start( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Start( GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Start_Union,
       'Finish( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Finish(GetIntersect ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Finish_Intersect,
       'Length( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union,
       'Period_Interval( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Period_Interval(GetIntersect (T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Intersect_Interval
  FROM Test_DT_Period T1, Test_DT_Period T2;
