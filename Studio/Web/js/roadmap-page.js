if(!api.isLoggedIn()){window.location.href='../login.html';}
else if(api._enforceRoleGuard()){}

Sidebar.init('roadmap');

var TIERS=[
  {n:1,t:'أساسيات البرمجة',d:'المتغيرات وأنواع البيانات الأساسية - الخطوة الأولى في عالم البرمجة',sk:['المتغيرات','أنواع البيانات','الإخراج','الإدخال','النصوص','الأعداد'],dt:['تعريف متغير','نوع بيانات','طباعة','إدخال','نص كامل','عدد صحيح']},
  {n:2,t:'المنطق والتحكم',d:'الشرطيات وحلقات التكرار - بناء قرارات ذكية',sk:['إذا/وإلا','جمل شرطية','حلقة لـ','حلقة طالما','القبض','المنطق'],dt:['شرط بسيط','شرط متعدد','تكرار for','تكرار while','break','&& و ||']},
  {n:3,t:'هيكل البيانات',d:'المصفوفات والقوائم - تنظيم البيانات بذكاء',sk:['المصفوفات','القوائم','الخرايط','إضافة','حذف','بحث'],dt:['تعريف مصفوفة','قائمة مرتبطة','خريطة key','push','pop','find']},
  {n:4,t:'البرمجة الكائنية',d:'الفئات والوراثة - بناء هياكل قوية وقابلة للتوسع',sk:['الفئات','الهياكل','الخصائص','الإنشاء','الوراثة','التغليف'],dt:['class new','struct','property','constructor','extends','private']},
  {n:5,t:'خوارزميات',d:'خوارزميات الفرز والبحث - حلول ذكية للمشكلات',sk:['فرز تصاعدي','فرز تنازلي','قلب المصفوفة','إزالة التكرارات','بحث خطي','دمج مصفوفتين'],dt:['sort up','sort down','reverse','unique','linear','merge']},
  {n:6,t:'المصفوفات المتقدمة',d:'عمليات متقدمة على المصفوفات - إتقان التعامل مع البيانات',sk:['إنشاء مصفوفة','الوصول للعناصر','طول المصفوفة','تعديل العناصر','إضافة عنصر','حذف عنصر'],dt:['create','access','length','modify','push','pop']},
  {n:7,t:'كائنية متقدمة',d:'البرمجة الكائنية المتقدمة - اتقان أعمق المفاهيم',sk:['الوراثة','الأصل','الوصول','المحمي','الصنف المجرد','الواجهة'],dt:['extends','super','access','protected','abstract','interface']}
];
var STATUS_AR={COMPLETED:'مكتمل',UNLOCKED:'متاح',LOCKED:'مقفل'};
function go(t){window.location.href='challenges.html?tier='+t;}

function updateStats(lessons){
  var comp=0,tc=0,cc=0,ep=0;
  lessons.forEach(function(l){
    if(l.status==='COMPLETED')comp++;
    tc+=l.totalExercises||0;
    cc+=l.completedExercises||0;
    ep+=l.earnedPoints||0;
  });
  var tot=lessons.length;
  document.getElementById('statCompleted').textContent=comp+'/'+tot;
  document.getElementById('statChallenges').textContent=cc+'/'+tc;
  document.getElementById('statXP').textContent=ep;

  var pct=tot>0?Math.round(comp/tot*100):0;
  document.getElementById('ringPct').textContent=pct+'%';
  document.getElementById('overallDetail').textContent=comp+' من '+tot+' وحدة';
  document.getElementById('overallDesc').textContent=pct>=100?'أحسنت! أنهيت المسار بالكامل':pct>=50?'ممتاز! واصل التقدم':pct>0?'بداية قوية - واصل التعلم':'ابدأ رحلتك في تعلم البرمجة';

  // Ring animation
  var circ=175.93;
  document.getElementById('ringFill').setAttribute('stroke-dashoffset',circ-(circ*pct/100));

  // Path progress
  var pathPct=tot>0?(comp/tot)*100:0;
  document.getElementById('pathProgress').style.height=pathPct+'%';

  // Hero badge - show current unlocked tier
  var currentTier='المستوى الأول';
  for(var i=lessons.length-1;i>=0;i--){
    if(lessons[i].status!=='LOCKED'){currentTier='المستوى '+arabicNum(i+1);break;}
  }
  document.getElementById('heroLevel').textContent=currentTier;
}

function arabicNum(n){
  var nums=['الأول','الثاني','الثالث','الرابع','الخامس','السادس','السابع'];
  return nums[n-1]||n;
}

function renderPath(lessons){
  var path=document.getElementById('rmPath');
  // Keep progress bar
  var progBar=path.querySelector('.rm-path-progress');
  path.innerHTML='';
  path.appendChild(progBar);

  for(var i=0;i<TIERS.length;i++){
    var ms=lessons.find(function(l){return l.order===i+1;})||{};
    var st=ms.status||'LOCKED';
    var cc=ms.completedExercises||0;
    var tc=ms.totalExercises||0;
    var ep=ms.earnedPoints||0;
    var tp=ms.totalPoints||0;
    var pct=tc>0?Math.round(cc/tc*100):0;
    var locked=st==='LOCKED';
    var sc=st.toLowerCase();

    // Milestone wrapper
    var m=document.createElement('div');
    m.className='rm-milestone';
    m.style.transitionDelay=(i*0.1)+'s';

    // Center dot
    var dot=document.createElement('div');
    dot.className='rm-dot '+sc;
    m.appendChild(dot);

    // Card
    var card=document.createElement('div');
    card.className='rm-card '+sc+(locked?' locked':'');

    // Skills HTML
    var skH='';
    for(var j=0;j<TIERS[i].sk.length;j++){
      skH+='<span class="rm-skill s'+(i+1)+'">'+TIERS[i].sk[j]+'</span>';
    }

    // Progress HTML
    var progH='';
    if(!locked&&tc>0){
      progH='<div class="rm-card-progress"><div class="rm-prog-row"><span>'+cc+' / '+tc+' تحدي</span><span>'+pct+'%</span></div><div class="rm-prog-bar"><div class="rm-prog-fill c'+(i+1)+'" style="width:'+pct+'%"></div></div></div>';
    }

    // Button
    var btnH='';
    if(locked){
      btnH='<button class="rm-btn-locked" disabled>مقفل</button>';
    }else if(st==='COMPLETED'){
      btnH='<button class="rm-btn-review" onclick="go('+(i+1)+')">مراجعة</button>';
    }else{
      btnH='<button class="rm-btn-start" onclick="go('+(i+1)+')">ابدأ التعلم</button>';
    }

    card.innerHTML=
      '<div class="rm-card-head">'+
        '<div class="rm-card-num c'+(i+1)+'">'+(st==='COMPLETED'?'&#10003;':(i+1))+'</div>'+
        '<div class="rm-card-meta"><h3>'+TIERS[i].t+'</h3><p>'+TIERS[i].d+'</p></div>'+
        '<span class="rm-card-badge '+sc+'">'+STATUS_AR[st]+'</span>'+
      '</div>'+
      '<div class="rm-card-skills">'+skH+'</div>'+
      progH+
      '<div class="rm-card-foot">'+
        '<span class="rm-xp"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"/></svg>'+ep+' / '+tp+' XP</span>'+
        btnH+
      '</div>';

    m.appendChild(card);
    path.appendChild(m);
  }

  // Intersection observer for scroll animation
  setTimeout(function(){
    var obs=new IntersectionObserver(function(entries){
      entries.forEach(function(e){
        if(e.isIntersecting){e.target.classList.add('visible');}
      });
    },{threshold:0.15});
    document.querySelectorAll('.rm-milestone').forEach(function(el){obs.observe(el);});
  },100);
}

function createParticles(){
  var c=document.getElementById('particles');
  for(var i=0;i<12;i++){
    var p=document.createElement('div');
    p.className='rm-particle';
    p.style.left=Math.random()*100+'%';
    p.style.top=Math.random()*100+'%';
    p.style.animationDelay=Math.random()*8+'s';
    p.style.animationDuration=(6+Math.random()*6)+'s';
    c.appendChild(p);
  }
}

function load(){
  createParticles();
  api.getRoadmap().then(function(res){
    var ls=(res.roadmap||res).lessons||[];
    if(!ls.length){
      ls=TIERS.map(function(m,i){
        return{order:i+1,title:m.t,status:i===0?'UNLOCKED':'LOCKED',completedExercises:0,totalExercises:0,completionRate:0,earnedPoints:0,totalPoints:0,challenges:[]};
      });
    }
    window._rd=ls;
    updateStats(ls);
    renderPath(ls);
  }).catch(function(){
    var ls=TIERS.map(function(m,i){
      return{order:i+1,title:m.t,status:i===0?'UNLOCKED':'LOCKED',completedExercises:0,totalExercises:0,completionRate:0,earnedPoints:0,totalPoints:0,challenges:[]};
    });
    window._rd=ls;
    updateStats(ls);
    renderPath(ls);
  });
}
load();
