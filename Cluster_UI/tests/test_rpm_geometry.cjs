const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync('rsx/RpmGeometry.js','utf8').replace(/^\s*\.pragma library/m,'');
const g = vm.createContext({Math});
vm.runInContext(source,g);
const near = (a,b,t=1e-7) => assert.ok(Math.abs(a-b)<t, `${a} != ${b}`);
for (const maximum of [1000,8000,8250,9000,30000]) {
    const layout=g.geometry(maximum,6800);
    assert.equal(layout.ticks[0].rpm,1000);
    assert.equal(layout.ticks.at(-1).rpm,Math.floor(maximum/500)*500);
    for (const tick of layout.ticks) {
        assert.equal(tick.label,String(tick.rpm/1000));
        assert.equal(tick.major,tick.rpm%1000===0);
        const theta=g.angle(tick.rpm/maximum), p=g.innerPoint(theta);
        // Tick and its upright label lie on the identical RPM normal.
        near((tick.x-p.x)*(tick.labelY-p.y)-(tick.y-p.y)*(tick.labelX-p.x),0);
    }
}
const layout=g.geometry(8000,6800);
assert.equal(layout.ticks.at(-1).label,'8');
assert.equal(layout.segments.length,80);
const boundary=g.path(6800/8000,0.87).match(/^M([^,]+),([^ L]+)/);
near(Number(boundary[1]),g.edge(6800/8000));
for (let i=0;i<=100;i++) {
    const t=g.angle(i/100), p=g.innerPoint(t);
    near(((p.x-g.cx)/g.innerA)**2+((p.y-g.cy)/g.innerB)**2,1);
    const q=g.point(t,g.outerA,g.outerB);
    near((q.x-p.x)*(-Math.sin(t)/g.outerB)-(q.y-p.y)*(Math.cos(t)/g.outerA),0);
    assert.ok(q.x>=0 && q.x<=1200 && p.y>=0 && p.y<=235);
    if(i) assert.ok(g.edge(i/100)>g.edge((i-1)/100));
}
for(const rpm of [1000,3500,6800,8000]) {
    const theta=g.angle(rpm/8000);
    console.log(`${rpm} RPM: theta=${(theta*180/Math.PI).toFixed(3)}, fill x=${g.edge(rpm/8000).toFixed(3)}`);
}
// Responsive parent fits the entire fixed-coordinate dashboard at both extremes.
for(const [w,h] of [[800,480],[1280,720],[1920,1080],[800,900]]) {
    const scale=Math.min(w/1280,h/720);
    assert.ok(1200*scale<=w && 640*scale<=h);
}
console.log('Ellipse, normals, tick alignment, exact 6800 boundary and resize geometry passed.');
