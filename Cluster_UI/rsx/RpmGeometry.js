// All path geometry is built only when scale/redline changes, never on RPM ticks.
.pragma library
var cx = 600, cy = 185, outerA = 580, outerB = 165
var innerA = 526, innerB = 98
var start = Math.PI * 165 / 180, end = Math.PI * 15 / 180
var samples = []
var total = 0
function point(theta, a, b) {
    return {x: cx + a*Math.cos(theta), y: cy - b*Math.sin(theta)}
}
// Intersect the outer ellipse normal with the inner ellipse. Each segment's
// sides therefore turn with the local slope, while both rims remain ellipses.
function innerPoint(theta) {
    var p=point(theta,outerA,outerB)
    var nx=Math.cos(theta)/outerA, ny=-Math.sin(theta)/outerB
    var norm=Math.hypot(nx,ny); nx/=norm; ny/=norm
    var px=p.x-cx, py=p.y-cy
    var aa=nx*nx/(innerA*innerA)+ny*ny/(innerB*innerB)
    var bb=-2*(px*nx/(innerA*innerA)+py*ny/(innerB*innerB))
    var cc=px*px/(innerA*innerA)+py*py/(innerB*innerB)-1
    var distance=(-bb-Math.sqrt(bb*bb-4*aa*cc))/(2*aa)
    return {x:p.x-distance*nx,y:p.y-distance*ny}
}
function init() {
    if (samples.length) return
    var previous = point(start, outerA, outerB)
    samples.push({t:start, length:0})
    for (var i=1; i<=2048; ++i) {
        var t = start + (end-start)*i/2048
        var p = point(t,outerA,outerB)
        total += Math.hypot(p.x-previous.x,p.y-previous.y)
        samples.push({t:t,length:total})
        previous = p
    }
}
function angle(fraction) {
    init()
    var distance = Math.max(0,Math.min(1,fraction))*total
    var low=0, high=samples.length-1
    while (high-low>1) {
        var mid=(low+high)>>1
        if (samples[mid].length < distance) low=mid; else high=mid
    }
    var weight=(distance-samples[low].length)/(samples[high].length-samples[low].length)
    return samples[low].t+(samples[high].t-samples[low].t)*weight
}
function edge(fraction) {
    var t=angle(fraction)
    // Both ellipses' x boundaries must be covered by the moving fill edge.
    return point(t,outerA,outerB).x
}
function path(from,to) {
    if (to<=from) return ""
    var a=angle(from), b=angle(to), result=""
    for (var i=0;i<=8;++i) {
        var p=point(a+(b-a)*i/8,outerA,outerB)
        result+=(i===0 ? "M" : " L")+p.x+","+p.y
    }
    for (var j=8;j>=0;--j) {
        var q=innerPoint(a+(b-a)*j/8)
        result+=" L"+q.x+","+q.y
    }
    return result+" Z"
}
function geometry(maximum,redline) {
    init()
    var segments=[], ticks=[]
    var count=80, gap=3/total, boundary=redline/maximum
    for (var i=0;i<count;++i) {
        var from=i/count+gap/2, to=(i+1)/count-gap/2
        segments.push({background:path(from,to), amber:path(from,Math.min(to,boundary)), red:path(Math.max(from,boundary),to)})
    }
    for (var rpm=1000;rpm<=maximum;rpm+=500) {
        var t=angle(rpm/maximum)
        var p=innerPoint(t)
        // Inward ellipse normal, so ticks point away from the band.
        var nx=-Math.cos(t)/outerA, ny=Math.sin(t)/outerB
        var magnitude=Math.hypot(nx,ny); nx/=magnitude; ny/=magnitude
        var major=rpm%1000===0, length=major ? 14 : 8
        ticks.push({rpm:rpm, label:String(rpm/1000), x:p.x+nx*7,y:p.y+ny*7,
            x2:p.x+nx*(7+length),y2:p.y+ny*(7+length),
            labelX:p.x+nx*37,labelY:p.y+ny*37,major:major})
    }
    return {segments:segments,ticks:ticks}
}
