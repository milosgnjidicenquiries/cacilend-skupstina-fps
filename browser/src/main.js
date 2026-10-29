import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.186.1/build/three.module.js';
import { GLTFLoader } from 'https://cdn.jsdelivr.net/npm/three@0.186.1/examples/jsm/loaders/GLTFLoader.js';

const $ = (id) => document.getElementById(id);
const canvas = $('view'), errorBox = $('error');
window.addEventListener('error', (event) => {
  errorBox.hidden = false;
  errorBox.textContent = 'Greška pri pokretanju igre: ' + (event.message || 'JavaScript greška') + '. Osveži stranicu kombinacijom Ctrl+F5 i pokušaj ponovo.';
});
window.addEventListener('unhandledrejection', (event) => {
  errorBox.hidden = false;
  const reason = event.reason && event.reason.message ? event.reason.message : String(event.reason || 'nepoznata greška');
  errorBox.textContent = 'Greška pri učitavanju igre: ' + reason + '. Proveri internet i osveži stranicu sa Ctrl+F5.';
});
let renderer;
try {
  renderer = new THREE.WebGLRenderer({canvas, antialias:true, powerPreference:'high-performance'});
} catch (error) {
  errorBox.hidden=false;
  errorBox.textContent='3D prikaz nije mogao da se pokrene. Uključi hardversko ubrzanje u Chrome/Edge browseru. '+error.message;
  throw error;
}
renderer.setPixelRatio(Math.min(devicePixelRatio || 1, 1.35));
renderer.setSize(innerWidth, innerHeight, false);
renderer.outputColorSpace=THREE.SRGBColorSpace;
renderer.toneMapping=THREE.ACESFilmicToneMapping;
renderer.toneMappingExposure=1.12;
renderer.shadowMap.enabled=true;
renderer.shadowMap.type=THREE.PCFSoftShadowMap;

const scene=new THREE.Scene();
scene.background=new THREE.Color(0x9da9a8);
scene.fog=new THREE.FogExp2(0x9da9a8,.008);
const camera=new THREE.PerspectiveCamera(74,innerWidth/innerHeight,.08,240);
camera.rotation.order='YXZ';
scene.add(camera);
scene.add(new THREE.HemisphereLight(0xdde8ee,0x514738,1.35));
const sun=new THREE.DirectionalLight(0xffe2b6,3.1);
sun.position.set(-28,43,20);sun.castShadow=true;sun.shadow.mapSize.set(2048,2048);
Object.assign(sun.shadow.camera,{left:-44,right:44,top:48,bottom:-48,near:.4,far:120});
sun.shadow.bias=-.00015;sun.shadow.normalBias=.03;scene.add(sun);
const fill=new THREE.DirectionalLight(0xc5d9e8,.65);fill.position.set(24,15,-25);scene.add(fill);

let seed=78237;
function rnd(){seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;}
const mats={}, solids=[], targets=[], traces=[];
function rgb(hex){const n=parseInt(hex.slice(1),16);return[(n>>16)&255,(n>>8)&255,n&255];}
function texture(kind,baseHex,detailHex){
 const size=384,c=document.createElement('canvas');c.width=c.height=size;
 const ctx=c.getContext('2d'),base=rgb(baseHex),detail=rgb(detailHex);
 const img=ctx.createImageData(size,size);
 for(let i=0;i<size*size;i++){
  const n=(rnd()-.5)*.42, row=Math.floor(i/size),col=i%size;
  const grain=kind==='wood'?Math.sin(col*.08+Math.sin(row*.025)*3)*.10:0;
  for(let k=0;k<3;k++)img.data[i*4+k]=Math.max(0,Math.min(255,base[k]+(detail[k]-base[k])*(.5+n+grain)));
  img.data[i*4+3]=255;
 }
 ctx.putImageData(img,0,0);
 if(kind==='asphalt'){
  for(let i=0;i<75;i++){ctx.strokeStyle=rnd()>.5?'#171a18aa':'#c3c0b044';ctx.lineWidth=rnd()*1.4;ctx.beginPath();let x=rnd()*size,y=rnd()*size;ctx.moveTo(x,y);for(let j=0;j<4;j++){x+=(rnd()-.5)*32;y+=rnd()*21;ctx.lineTo(x,y);}ctx.stroke();}
 }else if(kind==='concrete'){
  for(let i=0;i<650;i++){ctx.fillStyle=rnd()>.5?'#1b1a1620':'#fff5d619';ctx.fillRect(rnd()*size,rnd()*size,1+rnd()*3,1+rnd()*2);}
 }else if(kind==='canvas'){
  ctx.globalAlpha=.16;ctx.strokeStyle='#efe8d4';ctx.lineWidth=1;for(let i=0;i<size;i+=4){ctx.beginPath();ctx.moveTo(i,0);ctx.lineTo(i,size);ctx.stroke();ctx.beginPath();ctx.moveTo(0,i);ctx.lineTo(size,i);ctx.stroke();}ctx.globalAlpha=1;
 }else if(kind==='wood'){
  for(let i=0;i<45;i++){ctx.strokeStyle=rnd()>.5?'#20120755':'#e2b47822';ctx.lineWidth=rnd()*2+.25;ctx.beginPath();const y=rnd()*size;ctx.moveTo(0,y);ctx.bezierCurveTo(size*.3,y+(rnd()-.5)*12,size*.7,y+(rnd()-.5)*14,size,y+(rnd()-.5)*20);ctx.stroke();}
 }
 const t=new THREE.CanvasTexture(c);t.colorSpace=THREE.SRGBColorSpace;t.wrapS=t.wrapT=THREE.RepeatWrapping;t.anisotropy=8;return t;
}
function mat(name,base,detail,rough=.85,metal=0,kind='noise'){
 if(!mats[name]){const t=texture(kind,base,detail);mats[name]=new THREE.MeshStandardMaterial({map:t,bumpMap:(kind==='asphalt'||kind==='concrete'||kind==='canvas'||kind==='wood')?t:null,bumpScale:kind==='asphalt'?.025:.018,roughness:rough,metalness:metal});}
 return mats[name];
}
const M={
 road:mat('road','#303432','#73746d',.97,0,'asphalt'),
 concrete:mat('concrete','#79786f','#b3afa4',.92,0,'concrete'),
 stone:mat('stone','#a99a7e','#e2cfaa',.77,0,'concrete'),
 tent:mat('tent','#77715e','#c8bea5',.94,0,'canvas'),
 olive:mat('olive','#414535','#7b7f64',.91,0,'canvas'),
 metal:mat('metal','#282d29','#8e9187',.42,.62),
 dark:mat('dark','#161916','#3d423b',.86,0),
 wood:mat('wood','#4d301a','#a77a4b',.74,0,'wood'),
 grass:mat('grass','#263622','#718055',.98,0),
 carpet:mat('carpet','#50191d','#8a3b3e',.95,0,'canvas'),
 glass:new THREE.MeshPhysicalMaterial({color:0x293c42,metalness:.25,roughness:.18,clearcoat:.7,clearcoatRoughness:.16}),
 target:new THREE.MeshStandardMaterial({color:0x8a9789,metalness:.66,roughness:.34}),
 light:new THREE.MeshStandardMaterial({color:0xffcf77,emissive:0xff8b25,emissiveIntensity:2.5,roughness:.3}),
 tire:new THREE.MeshStandardMaterial({color:0x161916,roughness:.98}),
};
const boxGeo=new THREE.BoxGeometry(1,1,1);
function addMesh(g,m,x,y,z,sx,sy,sz,opts={}){
 const o=new THREE.Mesh(g,m);o.position.set(x,y,z);if(sx!==undefined)o.scale.set(sx,sy,sz);
 o.castShadow=opts.cast!==false;o.receiveShadow=opts.receive!==false;scene.add(o);return o;
}
function box(m,x,y,z,sx,sy,sz,yaw=0,opts={}){
 const o=addMesh(boxGeo,m,x,y,z,sx,sy,sz,opts);o.rotation.y=yaw;return o;
}
function cyl(m,x,y,z,r1,r2,h,segments=20,rx=0,ry=0,rz=0){
 const o=addMesh(new THREE.CylinderGeometry(r1,r2,h,segments),m,x,y,z);o.rotation.set(rx,ry,rz);return o;
}
function sph(m,x,y,z,sx,sy,sz){
 return addMesh(new THREE.SphereGeometry(1,16,12),m,x,y,z,sx,sy,sz);
}
function ground(m,x,z,w,d){const o=addMesh(new THREE.PlaneGeometry(w,d),m,x,-.06,z,1,1,1,{cast:false});o.rotation.x=-Math.PI/2;return o;}
function solid(x,z,hx,hz){solids.push({x,z,hx,hz});}
ground(M.road,0,-11,58,100);
// Approximate real-world block layout, scaled to a playable FPS area.
// Pionirski Park lies west/southwest of the National Assembly; the Main Post Office is east.
ground(M.grass,-38,3,61,39);
ground(M.grass,64,1,52,105);
ground(M.road,-38,-22,66,7.5);
ground(M.road,-38,25,68,8);
ground(M.road,-72,2,7.5,65);
ground(M.road,31,0,8,105);
ground(M.road,69,0,8,110);
box(M.concrete,-8,.04,0,1.1,.22,90);
box(M.concrete,27,.04,-11,1.3,.22,108);
for(let z=-55;z<=32;z+=10)box(M.dark,27.8,.012,z,.04,.01,4.5,0,{cast:false});

function barrier(x,z,yaw=0){
 x-=38;z+=11;
 const g=new THREE.Group();g.position.set(x,0,z);g.rotation.y=yaw;scene.add(g);
 for(const part of [
  [3.2,.45,.96,0,.24,0,M.concrete],[2.45,.55,.7,0,.72,0,M.concrete],[2.55,.07,.75,0,1.03,0,M.stone],[2.05,.1,.025,0,.77,.37,M.tent]
 ]){const o=new THREE.Mesh(boxGeo,part[6]);o.scale.set(part[0],part[1],part[2]);o.position.set(part[3],part[4],part[5]);o.castShadow=true;o.receiveShadow=true;g.add(o);}
 solid(x,z,1.8,.68);
}
function tent(x,z,yaw=0,variant=0){
 x-=38;z+=11;
 const g=new THREE.Group();g.position.set(x,0,z);g.rotation.y=yaw;scene.add(g);
 const fabric=variant%2?M.tent:M.olive;
 const wall=new THREE.Mesh(new THREE.BoxGeometry(3.5,1.65,2.6),fabric);wall.position.y=.85;wall.castShadow=true;wall.receiveShadow=true;g.add(wall);
 const door=new THREE.Mesh(new THREE.BoxGeometry(1.05,1.3,.04),M.dark);door.position.set(0,.66,1.32);g.add(door);
 const vertices=[-1.95,1.6,-1.45,0,2.58,-1.45,0,2.58,1.45,-1.95,1.6,1.45,0,2.58,-1.45,1.95,1.6,-1.45,1.95,1.6,1.45,0,2.58,1.45];
 const geo=new THREE.BufferGeometry();geo.setAttribute('position',new THREE.Float32BufferAttribute(vertices,3));geo.setIndex([0,1,2,0,2,3,4,5,6,4,6,7]);geo.computeVertexNormals();
 const roof=new THREE.Mesh(geo,new THREE.MeshStandardMaterial({map:fabric.map,roughness:.95,side:THREE.DoubleSide}));roof.castShadow=true;g.add(roof);
 for(const sx of [-1.75,1.75])for(const sz of [-1.2,1.2])cyl(M.metal,sx,.82,sz,.018,.018,1.65,8);
 solid(x,z,2.05,1.55);
}
function pallet(x,z,yaw=0){
 x-=38;z+=11;
 const g=new THREE.Group();g.position.set(x,.1,z);g.rotation.y=yaw;scene.add(g);
 const part=(m,a,b,c,d,e,f)=>{const o=new THREE.Mesh(boxGeo,m);o.position.set(a,b,c);o.scale.set(d,e,f);o.castShadow=true;o.receiveShadow=true;g.add(o);};
 part(M.wood,0,.055,0,1.35,.12,.83);for(let i=-2;i<=2;i++)part(M.wood,i*.26,.14,0,.12,.08,.83);
 for(let x of [-.43,0,.43])part(M.dark,x,.01,0,.16,.07,.76);solid(x,z,.8,.58);
}
function barrel(x,z){
 x-=38;z+=11;
 cyl(M.metal,x,.55,z,.4,.42,1.1,22);
 for(let y of [.23,.5,.81])cyl(M.dark,x,y,z,.425,.425,.055,22);
 cyl(M.olive,x,1.12,z,.38,.38,.035,22);solid(x,z,.5,.5);
}
function car(x,z,color,yaw=0){
 x-=38;z+=11;
 const g=new THREE.Group();g.position.set(x,0,z);g.rotation.y=yaw;scene.add(g);
 const paint=new THREE.MeshStandardMaterial({color:color,metalness:.45,roughness:.35});
 const p=(geo,m,a,b,c,sx,sy,sz)=>{const o=new THREE.Mesh(geo,m);o.position.set(a,b,c);o.scale.set(sx,sy,sz);o.castShadow=true;o.receiveShadow=true;g.add(o);return o;};
 p(boxGeo,paint,0,.63,0,4.2,.72,1.8);p(boxGeo,M.darkGlass,-.18,1.27,0,2.2,.68,1.48);p(boxGeo,paint,-.18,1.62,0,1.9,.07,1.43);
 for(const wx of [-1.42,1.42])for(const wz of [-.91,.91]){
  const w=p(new THREE.CylinderGeometry(.37,.37,.23,22),M.tire,wx,.4,wz,1,1,1);w.rotation.x=Math.PI/2;
  const hub=p(new THREE.CylinderGeometry(.17,.17,.24,14),M.metal,wx,.4,wz,1,1,1);hub.rotation.x=Math.PI/2;
 }
 for(const side of [-1,1]){
  p(boxGeo,M.glass,-.18,1.32,side*.745,.82,.39,.025);p(boxGeo,M.glass,.62,1.32,side*.745,.58,.39,.025);
  p(boxGeo,M.light,2.1,.76,side*.56,.035,.14,.22);
 }
 solid(x,z,2.2,1.15);
}
function tree(x,z,scale=1){
 cyl(M.wood,x,1.35*scale,z,.22*scale,.29*scale,2.7*scale,12);
 for(let i=0;i<14;i++){
  const a=rnd()*Math.PI*2,r=.3+rnd()*1.0,y=2.5+rnd()*2.15;
  const shades=[0x293c2b,0x30472f,0x3a4d32,0x46573a];
  const lm=new THREE.MeshStandardMaterial({color:shades[Math.floor(rnd()*shades.length)],roughness:.99});
  sph(lm,x+Math.cos(a)*r*scale,y*scale,z+Math.sin(a)*r*scale,(.45+rnd()*.45)*scale,(.5+rnd()*.5)*scale,(.45+rnd()*.45)*scale);
 }
}
function lamp(x,z){
 x-=38;z+=11;
 cyl(M.metal,x,3.2,z,.065,.085,6.4,12);
 const arm=cyl(M.metal,x+.55,6.2,z,.035,.035,1.1,10,0,0,Math.PI/2);
 box(M.dark,x+.95,6.15,z,.38,.1,.26);
}

function pathBox(x,z,w,d,material=M.concrete){
  return box(material,x,.005,z,w,.08,d,0,{cast:false,receive:true});
}
function parkTree(x,z,scale=1){
  tree(x,z,scale);
  solid(x,z,.45*scale,.45*scale);
}
function buildPionirskiPark(){
  // Formal park geometry: central walkways, oval flowerbeds, fountain and benches.
  pathBox(-38,3,56,2.25,M.stone);
  pathBox(-38,3,2.25,35,M.stone);
  pathBox(-38,-5,29,1.6,M.stone);
  pathBox(-38,11,29,1.6,M.stone);
  pathBox(-52,3,1.6,16,M.stone);
  pathBox(-24,3,1.6,16,M.stone);
  for(const x of [-58,-50,-26,-18]){
    box(M.grass,x,.005,3,3.5,.05,8.5,0,{cast:false});
    for(let i=0;i<5;i++)sph(i%2?M.olive:M.tent,x-1.2+i*.6,.18,3+(i%3-1)*2.4,.26,.22,.26);
  }
  // Central ornamental fountain.
  cyl(M.stone,-38,.15,3,4.45,4.8,.34,40);
  const water=new THREE.Mesh(new THREE.CylinderGeometry(3.98,3.98,.065,40),new THREE.MeshStandardMaterial({color:0x536b6b,metalness:.22,roughness:.24}));
  water.position.set(-38,.36,3);water.receiveShadow=true;scene.add(water);
  cyl(M.stone,-38,.58,3,.78,.92,.38,24);
  cyl(M.stone,-38,.92,3,.46,.58,.25,24);
  sph(M.light,-38,1.14,3,.19,.27,.19);
  solid(-38,3,4.7,4.7);
  // Mature trees around the park edges, away from the walkways.
  for(let i=0;i<6;i++){
    parkTree(-64,-11+i*5.1,.88+(i%3)*.08);
    parkTree(-12,-11+i*5.1,.92+(i%2)*.1);
  }
  for(let i=0;i<5;i++){
    parkTree(-58+i*9.5,-14,.85+(i%2)*.12);
    parkTree(-58+i*9.5,20,.85+(i%3)*.06);
  }
  // Benches face the paths; legs and slats are separate meshes for depth.
  for(const [x,z,rot] of [[-48,-4,0],[-28,-4,0],[-48,10,0],[-28,10,0],[-39,-10,Math.PI/2],[-39,16,Math.PI/2]]){
    const g=new THREE.Group();g.position.set(x,0,z);g.rotation.y=rot;scene.add(g);
    for(let i=0;i<3;i++)box(M.wood,x,0.52+i*.13,z,1.8,.1,.14,rot,{cast:true});
    for(const lx of [-.67,.67])box(M.dark,x+lx,.26,z,.1,.52,.12,rot);
  }
  // Low stone edging gives the park a clear border against surrounding streets.
  box(M.stone,-38,.08,-16.8,61,.16,.35);
  box(M.stone,-38,.08,22.7,61,.16,.35);
  box(M.stone,-68.4,.08,3, .35,.16,39.5);
  box(M.stone,-7.6,.08,3, .35,.16,39.5);
}
function frontWindow(x,y,z,w=1.2,h=1.5){
  box(M.glass,x,y,z,w,h,.085);
  box(M.stone,x,y-h/2-.08,z,w+.24,.1,.13);
  box(M.stone,x,y+h/2+.08,z,w+.24,.1,.13);
  box(M.stone,x-w/2-.07,y,z,.1,h+.22,.13);
  box(M.stone,x+w/2+.07,y,z,.1,h+.22,.13);
}
function sideWindow(x,y,z,w=1.2,h=1.5){
  box(M.glass,x,y,z,.085,h,w);
  box(M.stone,x,y-h/2-.08,z,.13,.1,w+.24);
  box(M.stone,x,y+h/2+.08,z,.13,.1,w+.24);
  box(M.stone,x,y,z-w/2-.07,.13,h+.22,.1);
  box(M.stone,x,y,z+w/2+.07,.13,h+.22,.1);
}
function buildPresidency(){
  // New Palace / Presidency, Andrićev venac 1 — west/south of the park.
  const x=-69,z=30;
  box(M.concrete,x,.45,z,21,.9,14);
  box(M.stone,x,7.7,z,19,14.5,12.4);
  box(M.stone,x,14.9,z,20,.55,13.1);
  box(M.stone,x,1.5,z-6.35,19.8,.45,.5);
  box(M.stone,x,13.9,z-6.5,20,.48,.55);
  // Central projecting façade and a copper-toned dome.
  box(M.stone,x,8.1,z-6.15,8.7,15.2,.75);
  box(M.stone,x,15.65,z-6.2,9.8,.8,1.2);
  const dome=new THREE.Mesh(new THREE.SphereGeometry(1,28,16,0,Math.PI*2,0,Math.PI/2),new THREE.MeshStandardMaterial({color:0x667568,metalness:.38,roughness:.52}));
  dome.position.set(x,16.1,z);dome.scale.set(3.25,2.25,3.15);dome.castShadow=true;scene.add(dome);
  box(M.stone,x,18.25,z,2.7,1.1,2.7);
  cyl(M.stone,x,19.1,z,.52,.68,.8,16);
  // Four storeys of tall windows facing north toward Pionirski Park.
  for(let floor=0;floor<4;floor++){
    const y=3.0+floor*3.15;
    for(const dx of [-7.2,-3.6,3.6,7.2])frontWindow(x+dx,y,z-6.28,1.3,1.8);
    if(floor>0)frontWindow(x,y,z-6.28,1.35,1.8);
  }
  for(const dx of [-6,-4,-2,0,2,4,6])cyl(M.stone,x+dx,7.4,z-6.6,.14,.17,7.6,14);
  box(M.dark,x,1.3,z-6.76,2.2,2.6,.18);
  for(let i=0;i<4;i++)box(M.stone,x,0.14+i*.22,z-7.7-i*.5,13+i*1.1,.2,.85);
  solid(x,z,9.8,6.2);
}
function buildMainPostOffice(){
  // Main Post Office Palace, Takovska 2 — east of the central street.
  const x=42,z=1;
  box(M.concrete,x,.5,z,27,.95,21);
  box(M.stone,x,7.25,z,25,13.4,18.5);
  box(M.stone,x,14.1,z,26,.65,19.4);
  box(M.stone,x,1.2,z-9.48,25.6,.5,.55);
  // West-facing monumental entrance and its Doric columns.
  box(M.stone,x-12.72,8,z,4.5,14.9,9.4);
  box(M.stone,x-12.8,15.4,z,5.4,1.3,10.4);
  box(M.stone,x-12.8,1.0,z,5.2,.8,10.2);
  for(let i=0;i<7;i++){
    const zz=z-7.5+i*2.5;
    cyl(M.stone,x-15.1,7.7,zz,.24,.28,11.5,18);
    cyl(M.stone,x-15.1,2.0,zz,.43,.43,.26,18);
    cyl(M.stone,x-15.1,13.4,zz,.42,.42,.3,18);
  }
  // Long window rhythm and stone lintels, characteristic of the monumental palace.
  for(let floor=0;floor<4;floor++){
    const y=3+floor*3.05;
    for(let i=0;i<7;i++){
      const zz=z-7.3+i*2.35;
      if(floor===0&&i===3){
        box(M.dark,x-15.1,2.2,zz,.12,2.25,1.35);
      } else sideWindow(x-12.58,y,zz,1.0,1.55);
    }
  }
  // Central tower / clock marker.
  box(M.stone,x-12.7,16.55,z,4.8,2.0,5.8);
  const clockFace=new THREE.Mesh(new THREE.CircleGeometry(.62,32),new THREE.MeshStandardMaterial({color:0xe4ddc7,roughness:.9}));
  clockFace.position.set(x-15.16,16.55,z);clockFace.rotation.y=-Math.PI/2;scene.add(clockFace);
  box(M.stone,x-12.7,18.2,z,3.4,.7,3.7);
  const crown=new THREE.Mesh(new THREE.ConeGeometry(1.8,2.4,4),M.stone);crown.position.set(x-12.7,19.65,z);crown.rotation.y=Math.PI/4;crown.castShadow=true;scene.add(crown);
  solid(x,z,12.7,9.4);
}
function cityBlock(x,z,w,d,floors,front='north'){
  const h=floors*3.0, facade=M.stone;
  box(M.concrete,x,.45,z,w+.9,.9,d+.8);
  box(facade,x,h/2,z,w,h,d);
  box(M.dark,x,h+.22,z,w+.6,.44,d+.6);
  box(M.stone,x,h-0.32,z,w+.2,.23,d+.2);
  if(front==='west'||front==='east'){
    const faceX=front==='west'?x-w/2-.07:x+w/2+.07;
    for(let fl=0;fl<floors;fl++)for(let j=0;j<Math.max(2,Math.floor(d/3));j++)sideWindow(faceX,1.7+fl*3,z-d/2+1.5+j*3,1.1,1.5);
  } else {
    const faceZ=front==='north'?z-d/2-.07:z+d/2+.07;
    for(let fl=0;fl<floors;fl++)for(let j=0;j<Math.max(2,Math.floor(w/3));j++)frontWindow(x-w/2+1.5+j*3,1.7+fl*3,faceZ,1.1,1.5);
  }
  solid(x,z,w/2,d/2);
}
function buildSurroundingStreets(){
  // Dense surrounding Belgrade blocks, broad streets, pavements and traffic lanes.
  ground(M.road,-38,47,100,8);
  ground(M.road,-38,-67,100,8);
  ground(M.road,-84,2,7,130);
  ground(M.road,88,0,7,130);
  for(const x of [-80,-66,-4,10,58,74])for(const z of [-52,41,57]){
    if((x===-4||x===10)&&z===41)continue;
    box(M.concrete,x,.015,z,12,.08,1.1,0,{cast:false});
  }
  cityBlock(-84,-3,13,25,5,'east');
  cityBlock(-84,35,13,21,4,'east');
  cityBlock(-84,-43,13,22,5,'east');
  cityBlock(-84,63,13,16,4,'east');
  cityBlock(81,-38,14,22,5,'west');
  cityBlock(81,-4,14,24,6,'west');
  cityBlock(81,33,14,23,4,'west');
  cityBlock(81,63,14,16,5,'west');
  cityBlock(-30,-70,22,12,4,'south');
  cityBlock(2,-70,24,12,5,'south');
  cityBlock(35,-70,22,12,4,'south');
  cityBlock(-24,55,22,13,4,'north');
  cityBlock(10,58,20,12,5,'north');
  cityBlock(49,53,22,14,4,'north');
}
function buildSoldierPlaceholder(x,z,yaw=0){
  // Temporary COD2-style human proxy while the separately licensed source meshes are imported.
  const g=new THREE.Group();g.position.set(x,0,z);g.rotation.y=yaw;scene.add(g);
  const uniform=new THREE.MeshStandardMaterial({color:0x555b48,roughness:.95});
  const vest=new THREE.MeshStandardMaterial({color:0x343a30,roughness:.92});
  const skin=new THREE.MeshStandardMaterial({color:0x98785d,roughness:.94});
  const helmet=new THREE.MeshStandardMaterial({color:0x424a39,roughness:.86});
  const boot=new THREE.MeshStandardMaterial({color:0x252822,roughness:.95});
  const part=(geo,m,px,py,pz,sx,sy,sz,rz=0)=>{const o=new THREE.Mesh(geo,m);o.position.set(px,py,pz);o.scale.set(sx,sy,sz);o.rotation.z=rz;o.castShadow=true;o.receiveShadow=true;g.add(o);return o;};
  part(boxGeo,uniform,-.18,.55,0,.22,1.0,.28,-.03);part(boxGeo,uniform,.18,.55,0,.22,1.0,.28,.03);
  part(boxGeo,boot,-.18,.13,-.025,.25,.22,.39);part(boxGeo,boot,.18,.13,-.025,.25,.22,.39);
  part(boxGeo,uniform,0,1.31,0,.62,.72,.36);
  part(boxGeo,vest,0,1.34,-.205,.53,.58,.16);
  part(boxGeo,vest,0,1.42,.2,.4,.46,.16);
  part(boxGeo,skin,0,1.82,-.005,.16,.16,.16);
  part(new THREE.SphereGeometry(1,16,12),skin,0,2.03,0,.22,.26,.22);
  part(new THREE.SphereGeometry(1,20,12,0,Math.PI*2,0,Math.PI/2),helmet,0,2.15,0,.28,.18,.28);
  part(boxGeo,helmet,0,2.13,-.02,.58,.06,.48);
  part(boxGeo,uniform,-.43,1.39,-.01,.2,.61,.22,.18);
  part(boxGeo,uniform,.43,1.39,-.01,.2,.61,.22,-.18);
  part(boxGeo,skin,-.5,1.06,-.11,.15,.24,.16,.12);
  part(boxGeo,skin,.5,1.06,-.11,.15,.24,.16,-.12);
  part(boxGeo,vest,0,1.04,.24,.42,.44,.22);
  // A compact rifle silhouette; non-interactive background character.
  part(boxGeo,vest,.22,1.23,-.31,.42,.095,.1,-.15);
  part(boxGeo,boot,.39,1.25,-.47,.32,.045,.045);
  for(let i=0;i<4;i++)part(boxGeo,uniform,-.22+i*.145,1.35,-.3,.08,.1,.04);
  return g;
}

function buildAssembly(){
 box(M.stone,0,7.1,-40,48,14.2,3.5);
 box(M.stone,0,14.55,-40,50,.55,4.1);
 box(M.dark,0,15.05,-40,49,.11,3.9);
 box(M.stone,0,15.6,-40,48,.75,3.5);
 box(M.stone,0,2.15,-37.6,29,4.3,1.7);
 for(let i=0;i<9;i++){
  const x=-14+i*3.5;
  cyl(M.stone,x,7.35,-36.75,.43,.55,10,24);
  cyl(M.stone,x,12.52,-36.75,.62,.62,.37,24);
  cyl(M.stone,x,2.3,-36.75,.65,.65,.45,24);
 }
 const sh=new THREE.Shape();sh.moveTo(-11,0);sh.lineTo(11,0);sh.lineTo(0,5.1);sh.closePath();
 const ped=new THREE.Mesh(new THREE.ExtrudeGeometry(sh,{depth:.45,bevelEnabled:true,bevelSegments:2,steps:1,bevelSize:.08,bevelThickness:.08}),M.stone);
 ped.position.set(0,15.35,-36.1);ped.castShadow=true;scene.add(ped);
 for(let level=0;level<3;level++)for(let i=0;i<13;i++){
  const x=-21+i*3.5,y=4.25+level*2.65;
  if(level===0&&Math.abs(x)<3.8)continue;
  box(M.glass,x,y,-38.23,1.65,1.55,.07);
  box(M.stone,x,y-.87,-38.13,1.85,.12,.14);box(M.stone,x,y+.86,-38.13,1.85,.1,.14);
  box(M.stone,x-.89,y,-38.13,.12,1.8,.14);box(M.stone,x+.89,y,-38.13,.12,1.8,.14);
 }
 for(let i=0;i<7;i++)box(M.stone,0,.13+i*.23,-33.8-i*.62,18+i*1.2,.2,.8);
 // Entrance opening is intentionally left clear; blocked() handles the facade sides.
}
function buildInterior(){
 const x=78,z=-2;
 box(M.carpet,x,-.1,z,25,.14,37);
 box(M.stone,x,5.2,z-18.5,25,10.4,.6);
 box(M.stone,x-12.4,5.2,z,.6,10.4,37);
 box(M.stone,x+12.4,5.2,z,.6,10.4,37);
 box(M.stone,x,10.4,z,25,.5,37);
 box(M.stone,x-8,4.7,z+18.5,.7,9.4,.6);box(M.stone,x+8,4.7,z+18.5,.7,9.4,.6);box(M.stone,x,9.7,z+18.5,15,.6,.6);
 for(let i=0;i<11;i++)cyl(M.stone,x-11+i*2.2,4.4,z-16,.22,.28,8.8,16);
 box(M.wood,x,1,z-14,9.5,1.6,1.15);box(M.carpet,x,1.85,z-14,9.8,.13,1.35);
 for(let row=0;row<5;row++){
  const yy=z+7-row*4.3;
  for(let i=0;i<8;i++){
   const xx=x-10+i*2.85;
   box(M.wood,xx,.82,yy,1.72,.15,.78);
   box(M.wood,xx,.43,yy-.12,.11,.72,.11);
   box(M.carpet,xx,.56,yy+1.05,1.08,.8,.1);
   box(M.carpet,xx,.98,yy+1.4,1.08,.15,.1);
  }
 }
 for(let i=0;i<5;i++){cyl(M.metal,x-8+i*4,8.5,z-11,.06,.06,.7,12);sph(M.light,x-8+i*4,8.05,z-11,.28,.13,.28);}
}
buildAssembly();buildInterior();buildPionirskiPark();buildPresidency();buildMainPostOffice();buildSurroundingStreets();
for(let i=0;i<12;i++){tree(-34+(i%3),-45+i*7.5,.85+rnd()*.3);tree(34-(i%3),-45+i*7.5,.9+rnd()*.32);}
buildSoldierPlaceholder(-47,2,.25);buildSoldierPlaceholder(-29,8,-.55);buildSoldierPlaceholder(-49,-8,.45);
for(let row=0;row<4;row++)for(let col=0;col<2;col++){
 tent(-8.5-col*3.3,4-row*8,row%2?.07:-.04,row+col);
 tent(8.5+col*3.3,2-row*8,row%2?-.07:.04,row+col+1);
}
for(let i=0;i<14;i++){const z=18-i*5.2;if(i%2===0)barrier(-17.2,z,.02);if(i%3!==0)barrier(17.2,z,-.02);}
for(let i=0;i<8;i++)pallet(-14+(i%3)*1.2,3-Math.floor(i/3)*6,(i%2)*.1);
for(let i=0;i<5;i++)barrel(14,11-i*5);
car(-22,4,0x666c67,.02);car(22,-22,0x28302e,-.03);
lamp(-24,-12);lamp(24,-26);

// Non-human robotic practice targets: only these count toward the score.
function makeTarget(x,z,id){
 const g=new THREE.Group();g.position.set(x,0,z);scene.add(g);
 const parts=[];
 function part(geo,m,px,py,pz,sx,sy,sz){
  const o=new THREE.Mesh(geo,m);o.position.set(px,py,pz);o.scale.set(sx,sy,sz);o.castShadow=true;o.receiveShadow=true;o.userData.practiceTarget=g;g.add(o);parts.push(o);return o;
 }
 part(new THREE.BoxGeometry(1,1,1),M.target,0,1.08,0,.58,.86,.34);
 part(new THREE.SphereGeometry(1,16,12),M.target,0,1.83,0,.25,.25,.25);
 part(new THREE.BoxGeometry(1,1,1),M.dark,0,1.1,-.20,.34,.47,.06);
 part(new THREE.SphereGeometry(1,16,12),M.light,0,1.15,-.26,.11,.14,.045);
 part(new THREE.BoxGeometry(1,1,1),M.target,-.4,1.12,0,.18,.64,.2);
 part(new THREE.BoxGeometry(1,1,1),M.target,.4,1.12,0,.18,.64,.2);
 part(new THREE.BoxGeometry(1,1,1),M.target,-.18,.36,0,.2,.65,.22);
 part(new THREE.BoxGeometry(1,1,1),M.target,.18,.36,0,.2,.65,.22);
 const t={group:g,parts,score:0,active:true,id,baseX:x,baseZ:z,phase:rnd()*6};targets.push(t);return t;
}
const targetSpawns=[[-24,11],[-19,0],[-24,-7],[-53,12],[-48,0],[-45,-11],[-35,17],[-35,-12],[-17,8],[-55,-10],[-29,18],[-56,7]];
targetSpawns.forEach((p,i)=>makeTarget(p[0],p[1],i));

// View-model assembled from physical materials to replace the floating toy silhouette.
const weapon=new THREE.Group();camera.add(weapon);weapon.position.set(.34,-.31,-.58);
function gunPart(g,m,x,y,z,sx,sy,sz){const o=new THREE.Mesh(g,m);o.position.set(x,y,z);o.scale.set(sx,sy,sz);o.castShadow=false;weapon.add(o);return o;}
const gunMetal=new THREE.MeshStandardMaterial({color:0x252b26,metalness:.78,roughness:.29});
const gunPoly=new THREE.MeshStandardMaterial({color:0x32392f,metalness:.14,roughness:.66});
const glove=new THREE.MeshStandardMaterial({color:0x57513f,roughness:.94});
gunPart(boxGeo,gunMetal,0,0,0,.42,.12,.13);gunPart(boxGeo,gunPoly,-.18,-.01,.08,.22,.1,.14,.06);
gunPart(boxGeo,gunPoly,.12,.01,-.06,.22,.07,.075);gunPart(boxGeo,gunMetal,.02,.084,-.005,.29,.018,.03);
gunPart(boxGeo,gunPoly,-.05,-.095,.045,.09,.17,.1,-.12);
const gunBarrel=new THREE.Mesh(new THREE.CylinderGeometry(.022,.028,.42,14),gunMetal);gunBarrel.rotation.x=Math.PI/2;gunBarrel.position.set(.28,.01,-.19);weapon.add(gunBarrel);
const hand1=new THREE.Mesh(new THREE.SphereGeometry(.075,16,12),glove);hand1.position.set(.02,-.1,.02);hand1.scale.set(1,.8,1.3);weapon.add(hand1);
const hand2=hand1.clone();hand2.position.set(.25,-.09,-.12);weapon.add(hand2);
const flash=new THREE.Mesh(new THREE.ConeGeometry(.065,.23,8),new THREE.MeshBasicMaterial({color:0xffd89a,transparent:true,opacity:.9}));flash.rotation.x=-Math.PI/2;flash.position.set(.29,.01,-.43);flash.visible=false;weapon.add(flash);
const flashLight=new THREE.PointLight(0xffca81,0,5);flashLight.position.set(.29,.01,-.35);weapon.add(flashLight);

const rifleAssetUrl='https://raw.githubusercontent.com/petroulacl/fps-asset-kit/main/weapons/flat_guns_west/Flat%20Guns%20West/GLB/Rifle_Assault_West.glb';
let rifleAssetLoaded=false;
const fallbackWeaponChildren=[...weapon.children].filter(child=>child!==flash&&child!==flashLight);
fallbackWeaponChildren.forEach(child=>child.visible=false);
new GLTFLoader().load(rifleAssetUrl,gltf=>{
  const rifle=gltf.scene;
  const box3=new THREE.Box3().setFromObject(rifle);
  const size=new THREE.Vector3();box3.getSize(size);
  const center=new THREE.Vector3();box3.getCenter(center);
  rifle.position.sub(center);
  const longestAxis=size.x>=size.y&&size.x>=size.z?'x':(size.y>=size.z?'y':'z');
  if(longestAxis==='x')rifle.rotation.y=Math.PI/2;
  else if(longestAxis==='y')rifle.rotation.x=-Math.PI/2;
  const rotatedBox=new THREE.Box3().setFromObject(rifle);
  const rotatedSize=new THREE.Vector3();rotatedBox.getSize(rotatedSize);
  const length=Math.max(rotatedSize.x,rotatedSize.y,rotatedSize.z)||1;
  rifle.scale.setScalar(.82/length);
  const scaledBox=new THREE.Box3().setFromObject(rifle);
  const scaledCenter=new THREE.Vector3();scaledBox.getCenter(scaledCenter);
  rifle.position.sub(scaledCenter);
  rifle.position.set(.02,-.025,-.04);
  rifle.traverse(o=>{if(o.isMesh){o.castShadow=false;o.frustumCulled=false;}});
  weapon.add(rifle);rifleAssetLoaded=true;
},undefined,error=>{
  console.warn('Optional CC0 rifle GLB failed to load; using built-in weapon.',error);
  fallbackWeaponChildren.forEach(child=>child.visible=true);
});

const player={pos:new THREE.Vector3(-61,0,8),yaw:-0.78,pitch:0,health:100,ammo:30,reserve:120,hits:0,inside:false,saved:new THREE.Vector3(),sens:.0017,locked:false,jumpHeight:0,jumpVelocity:0,onGround:true};
const keys=new Set(),raycaster=new THREE.Raycaster();raycaster.far=85;
let active=false,reloadTime=0,reloading=false,shotDelay=0,recoil=0,flashTime=0,toastTime=0,hitTime=0;
camera.position.set(player.pos.x,1.68,player.pos.z);camera.rotation.set(player.pitch,player.yaw,0,'YXZ');
function toast(text){$('toast').textContent=text;$('toast').classList.add('show');toastTime=1.9;}
function updateHud(){
 $('health').textContent=String(Math.round(player.health)).padStart(3,'0');$('healthbar').style.width=player.health+'%';
 $('kills').textContent=String(player.hits).padStart(2,'0');$('ammo').textContent=String(player.ammo).padStart(2,'0');$('reserve').textContent=String(player.reserve).padStart(3,'0');
 $('reload').textContent=reloading?'RELOADING…':'';$('mission').textContent=player.inside?'OBJEKAT: SKUPŠTINA':'OPERACIJA: PRILAZ';
 $('objective').textContent=player.inside?'ISTRAŽI SKUPŠTINSKU SALU':'POGODI ROBOTSKE TRENING METE';
}
function nearDoor(){return !player.inside&&Math.abs(player.pos.z+32.5)<5&&Math.abs(player.pos.x)<5;}
function hint(){if(nearDoor())$('hint').textContent='[ E ] UĐI U SKUPŠTINU';else if(player.inside&&player.pos.z>10)$('hint').textContent='[ E ] IZAĐI NA PLATO';else $('hint').textContent='';}
function interact(){
 if(!player.inside&&nearDoor()){player.saved.copy(player.pos);player.inside=true;player.pos.set(78,0,8);player.yaw=0;player.pitch=0;player.jumpHeight=0;player.jumpVelocity=0;player.onGround=true;camera.position.set(78,1.68,8);camera.rotation.set(0,0,0);toast('ULAZAK U SKUPŠTINU');updateHud();}
 else if(player.inside&&player.pos.z>10){player.inside=false;player.pos.copy(player.saved);player.yaw=0;player.pitch=0;player.jumpHeight=0;player.jumpVelocity=0;player.onGround=true;camera.position.set(player.pos.x,1.68,player.pos.z);camera.rotation.set(0,0,0);toast('POVRATAK NA PLATO');updateHud();}
}
function blocked(x,z){
 if(player.inside){return x<66.8||x>89.2||z< -19||z>18.5;}
 if(x < -96 || x > 96 || z > 78 || z < -84)return true;
 if(Math.abs(z+40)<2.7&&Math.abs(x)<25.5&&Math.abs(x)>4.2)return true;
 for(const s of solids)if(Math.abs(x-s.x)<s.hx+.38&&Math.abs(z-s.z)<s.hz+.38)return true;
 return false;
}
let sound=null;
function shotSound(){
 try{sound ||= new AudioContext();if(sound.state==='suspended')sound.resume();const now=sound.currentTime;
 const buffer=sound.createBuffer(1,Math.floor(sound.sampleRate*.1),sound.sampleRate),data=buffer.getChannelData(0);for(let i=0;i<data.length;i++)data[i]=(Math.random()*2-1)*Math.pow(1-i/data.length,3);
 const src=sound.createBufferSource();src.buffer=buffer;const low=sound.createBiquadFilter();low.type='lowpass';low.frequency.setValueAtTime(1650,now);low.frequency.exponentialRampToValueAtTime(220,now+.1);
 const gain=sound.createGain();gain.gain.setValueAtTime(.14,now);gain.gain.exponentialRampToValueAtTime(.001,now+.105);src.connect(low);low.connect(gain);gain.connect(sound.destination);src.start();src.stop(now+.11);
 }catch{}
}
function tracer(a,b){
 const g=new THREE.BufferGeometry().setFromPoints([a,b]),l=new THREE.Line(g,new THREE.LineBasicMaterial({color:0xffdc97,transparent:true,opacity:.75}));scene.add(l);traces.push({line:l,time:.07});
}
function fire(){
 if(!active||!player.locked||reloading||shotDelay>0)return;
 if(player.ammo<=0){toast('PRAZAN OKVIR — PRITISNI R');return;}
 player.ammo--;shotDelay=.13;recoil=Math.min(recoil+.04,.15);flash.visible=true;flashLight.intensity=12;flashTime=.045;shotSound();
 raycaster.setFromCamera(new THREE.Vector2(0,0),camera);const hits=raycaster.intersectObjects(scene.children.filter(object=>object!==camera),true);
 let end=camera.position.clone().add(raycaster.ray.direction.clone().multiplyScalar(60));
 if(hits.length){end=hits[0].point;let o=hits[0].object;while(o&&o!==scene){if(o.userData&&o.userData.practiceTarget){const t=targets.find(q=>q.group===o.userData.practiceTarget);if(t&&t.active){t.active=false;t.group.visible=false;player.hits++;$('hit').classList.add('on');hitTime=.12;toast('META POGOĐENA');setTimeout(()=>{t.group.position.set(t.baseX,0,t.baseZ);t.group.visible=true;t.active=true;},2200);}break;}o=o.parent;}}
 tracer(camera.position.clone().add(raycaster.ray.direction.clone().multiplyScalar(.5)),end);updateHud();
}
function reload(){
 if(!active||reloading||player.ammo>=30||player.reserve<=0)return;
 reloading=true;reloadTime=1.35;toast('PROMENA OKVIRA');updateHud();
}
$('start').addEventListener('click',()=>{active=true;$('screen').classList.add('hide');canvas.requestPointerLock?.();});
document.addEventListener('pointerlockchange',()=>{player.locked=document.pointerLockElement===canvas;if(player.locked){active=true;$('screen').classList.add('hide');}else if(active){active=false;$('screen').classList.remove('hide');$('start').textContent='▶   NASTAVI MISIJU';}});
document.addEventListener('mousemove',e=>{if(!player.locked)return;player.yaw-=e.movementX*player.sens*Number($('sens').value);player.pitch=THREE.MathUtils.clamp(player.pitch-e.movementY*player.sens*Number($('sens').value),-1.18,1.18);});
document.addEventListener('keydown',e=>{const k=e.key.toLowerCase();if(['w','a','s','d',' ','shift','r','e'].includes(k))e.preventDefault();if(k===' '&&active&&player.locked&&player.onGround){player.jumpVelocity=5.1;player.onGround=false;}keys.add(k);if(k==='r')reload();if(k==='e')interact();});
document.addEventListener('keyup',e=>keys.delete(e.key.toLowerCase()));
document.addEventListener('mousedown',e=>{if(e.button===0)fire();});
$('settings').addEventListener('click',()=>$('settingsPanel').classList.toggle('open'));$('closeSettings').addEventListener('click',()=>$('settingsPanel').classList.remove('open'));
$('quality').addEventListener('change',e=>{renderer.setPixelRatio(Math.min(devicePixelRatio||1,Number(e.target.value)));renderer.setSize(innerWidth,innerHeight,false);});
addEventListener('resize',()=>{camera.aspect=innerWidth/innerHeight;camera.updateProjectionMatrix();renderer.setSize(innerWidth,innerHeight,false);});

const clock=new THREE.Clock();let ftime=0,frames=0;
function animate(){
 requestAnimationFrame(animate);const dt=Math.min(clock.getDelta(),.04);
 if(active&&player.locked){
  if(!player.onGround){player.jumpVelocity-=14*dt;player.jumpHeight+=player.jumpVelocity*dt;if(player.jumpHeight<=0){player.jumpHeight=0;player.jumpVelocity=0;player.onGround=true;}}
  let f=0,r=0;if(keys.has('w'))f++;if(keys.has('s'))f--;if(keys.has('d'))r++;if(keys.has('a'))r--;
  const sprint=keys.has('shift'),speed=sprint?8:4.6,mag=Math.hypot(f,r)||1;f/=mag;r/=mag;
  const dx=(-Math.sin(player.yaw)*f+Math.cos(player.yaw)*r)*speed*dt,dz=(-Math.cos(player.yaw)*f-Math.sin(player.yaw)*r)*speed*dt;
  if(!blocked(player.pos.x+dx,player.pos.z))player.pos.x+=dx;
  if(!blocked(player.pos.x,player.pos.z+dz))player.pos.z+=dz;
  camera.position.set(player.pos.x,1.68+player.jumpHeight+Math.sin(performance.now()*.012)*(Math.abs(f)+Math.abs(r)>0?.018:0),player.pos.z);
  camera.rotation.set(player.pitch+recoil,player.yaw,Math.sin(performance.now()*.008)*(Math.abs(f)+Math.abs(r)>0?.004:0),'YXZ');
  weapon.position.y=-.31+Math.sin(performance.now()*.012)*(Math.abs(f)+Math.abs(r)>0?.006:0)-recoil*.35;
  recoil=Math.max(0,recoil-dt*.5);shotDelay=Math.max(0,shotDelay-dt);
  if(reloading){reloadTime-=dt;if(reloadTime<=0){const n=Math.min(30-player.ammo,player.reserve);player.ammo+=n;player.reserve-=n;reloading=false;updateHud();}}
  if(flashTime>0){flashTime-=dt;if(flashTime<=0){flash.visible=false;flashLight.intensity=0;}}
  hint();
 }
 for(let i=traces.length-1;i>=0;i--){const t=traces[i];t.time-=dt;t.line.material.opacity=Math.max(0,t.time/.07);if(t.time<=0){scene.remove(t.line);t.line.geometry.dispose();t.line.material.dispose();traces.splice(i,1);}}
 if(hitTime>0){hitTime-=dt;if(hitTime<=0)$('hit').classList.remove('on');}
 if(toastTime>0){toastTime-=dt;if(toastTime<=0)$('toast').classList.remove('show');}
 ftime+=dt;frames++;if(ftime>.75){if(player.locked)$('mission').textContent=(player.inside?'OBJEKAT: SKUPŠTINA':'OPERACIJA: PRILAZ')+' · '+Math.round(frames/ftime)+' FPS';ftime=0;frames=0;}
 renderer.render(scene,camera);
}
updateHud();animate();
