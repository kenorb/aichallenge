package com.aicontest.visualizer.js.dom;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.awt.TexturePaint;
import java.awt.font.LineMetrics;
import java.awt.geom.Arc2D;
import java.awt.geom.Path2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.StringTokenizer;

public class CanvasRenderingContext2d extends RenderingContext2dState {
	private final HTMLCanvasElement canvas;
	private Graphics2D gfx;
	private Deque<RenderingContext2dState> stack;
	private static final Point2D ONE_PIXEL = new Point2D.Double(0.0D, -1.0D);

	private boolean fontChanged = true;
	private boolean fillChanged = true;
	private boolean strokeChanged = true;
	private boolean drawn = true;

	public CanvasRenderingContext2d(HTMLCanvasElement canvas, BufferedImage pixmap) {
		this.canvas = canvas;
		updatePixmap(pixmap);
		stack = new ArrayDeque<RenderingContext2dState>();
	}

	public HTMLCanvasElement getCanvas() {
		return canvas;
	}

	public void restore() {
		setFrom((RenderingContext2dState) stack.removeLast());
		fontChanged = true;
		fillChanged = true;
		strokeChanged = true;
		gfx.setTransform(transform);
		gfx.setClip(clip);
	}

	public void save() {
		stack.add(new RenderingContext2dState(this));
	}

	public void rotate(double angle) {
		gfx.rotate(angle);
		transform = gfx.getTransform();
	}

	public void translate(double x, double y) {
		gfx.translate(x, y);
		transform = gfx.getTransform();
	}

	private Object setStyle(Object style) throws Exception {
		if ((style instanceof String)) {
			String color = (String) style;
			if (color.startsWith("rgb(")) {
				color = color.substring(4, color.length() - 1);
				StringTokenizer st = new StringTokenizer(color, ",");
				int r = Integer.parseInt(st.nextToken().trim());
				int g = Integer.parseInt(st.nextToken().trim());
				int b = Integer.parseInt(st.nextToken().trim());
				style = new Color(r, g, b);
			} else if (color.startsWith("rgba(")) {
				color = color.substring(5, color.length() - 1);
				StringTokenizer st = new StringTokenizer(color, ",");
				float r = Integer.parseInt(st.nextToken().trim()) / 255.0F;
				float g = Integer.parseInt(st.nextToken().trim()) / 255.0F;
				float b = Integer.parseInt(st.nextToken().trim()) / 255.0F;
				float a = Float.parseFloat(st.nextToken().trim());
				style = new Color(r, g, b, a);
			} else if (color.length() == 7) {
				int r = Integer.parseInt(color.substring(1, 3), 16);
				int g = Integer.parseInt(color.substring(3, 5), 16);
				int b = Integer.parseInt(color.substring(5, 7), 16);
				style = new Color(r, g, b);
			} else if (color.length() == 4) {
				int r = 17 * Character.digit(color.charAt(1), 16);
				int g = 17 * Character.digit(color.charAt(2), 16);
				int b = 17 * Character.digit(color.charAt(3), 16);
				style = new Color(r, g, b);
			} else {
				throw new Exception("cannot parse paint style: " + style);
			}
		}
		return style;
	}

	private void setStrokeStyle() throws Exception {
		if (strokeChanged) {
			Object strokeObj = setStyle(strokeStyle);
			if ((strokeObj instanceof Color)) {
				gfx.setColor((Color) strokeObj);
				gfx.setStroke(new BasicStroke(lineWidth));
			} else {
				gfx.setStroke((Stroke) strokeObj);
			}
			strokeChanged = false;
		}
	}

	private void setFillStyle() throws Exception {
		if (fillChanged) {
			Object fillObj = setStyle(fillStyle);
			if ((fillObj instanceof CanvasPattern)) {
				BufferedImage cp = ((CanvasPattern) fillObj).getPattern();

				Point2D p = transform.transform(ONE_PIXEL, null);
				Rectangle2D.Double anchor = new Rectangle2D.Double(0.0D, p.getX(), cp.getWidth(), cp.getHeight());
				gfx.setPaint(new TexturePaint(cp, anchor));
			} else {
				gfx.setPaint((Paint) fillObj);
			}
			fillChanged = false;
		}
	}

	public CanvasPattern createPattern(HTMLImageElement image, String repetition) {
		return new CanvasPattern(image.getImage(), repetition, canvas.getImageObserver());
	}

	public CanvasPattern createPattern(HTMLCanvasElement cvs, String repetition) {
		return new CanvasPattern(cvs.getPixmap(), repetition, canvas.getImageObserver());
	}

	public void clearRect(double x, double y, double w, double h) {
		gfx.clearRect((int) x, (int) y, (int) w, (int) h);
		drawn = true;
	}

	public void fillRect(double x, double y, double w, double h) throws Exception {
		setFillStyle();
		Path2D.Double oldPath = path;
		beginPath();
		rect(x, y, w, h);
		gfx.fill(path);
		path = oldPath;
		drawn = true;
	}

	public void beginPath() {
		path = new Path2D.Double();
	}

	public void closePath() {
		if (path != null)
			path.closePath();
	}

	public void moveTo(double x, double y) {
		if (path != null) {
			path.moveTo(x, y);
		}
		this.x = x;
		this.y = y;
	}

	public void lineTo(double x, double y) throws Exception {
		if (path == null) {
			setStrokeStyle();
			gfx.drawLine((int) this.x, (int) this.y, (int) x, (int) y);
		} else {
			path.lineTo(x, y);
		}
		this.x = x;
		this.y = y;
	}

	public void rect(double x, double y, double w, double h) {
		if (path != null) {
			path.moveTo(x, y);
			path.lineTo(x, y + h);
			path.lineTo(x + w, y + h);
			path.lineTo(x + w, y);
			path.lineTo(x, y);
		}
	}

	public void arc(double x, double y, double radius, double startAngle, double endAngle) {
		arc(x, y, radius, startAngle, endAngle, false);
	}

	public void arc(double x, double y, double radius, double startAngle, double endAngle, boolean antiClockwise) {
		if ((path != null) && (!antiClockwise)) {
			path.append(new Arc2D.Double(x - radius, y - radius, 2.0D * radius, 2.0D * radius, -startAngle * 180.0D / 3.141592653589793D, (startAngle - endAngle) * 180.0D / 3.141592653589793D, 0), true);
		}
	}

	public void fill() throws Exception {
		setFillStyle();
		gfx.fill(path);
		drawn = true;
	}

	public void stroke() throws Exception {
		setStrokeStyle();
		gfx.draw(path);
		drawn = true;
	}

	public void clip() {
		gfx.clip(path);
		if (path != null)
			clip = ((Path2D.Double) path.clone());
	}

	private void setFont() {
		if (fontChanged) {
			String name = "sans-serif";
			int style = 0;
			int pt = 7;
			StringTokenizer st = new StringTokenizer(font);
			int ct = st.countTokens();
			for (int i = 0; i < ct; i++) {
				String token = st.nextToken();
				if (i == ct - 1)
					name = token;
				else if (i == ct - 2) {
					if (token.endsWith("px"))
						pt = Integer.parseInt(token.substring(0, token.length() - 2));
				} else if (token.equals("bold"))
					style |= 1;
				else if (token.equals("italic")) {
					style |= 2;
				}
			}
			gfx.setFont(new Font(name, style, pt));
			fontChanged = false;
		}
	}

	public void fillText(String text, float x, float y) throws Exception {
		setFillStyle();
		setFont();
		LineMetrics metrics = gfx.getFontMetrics().getLineMetrics(text, gfx);
		if (textBaseline.equals("top"))
			y += metrics.getAscent();
		else if (textBaseline.equals("middle"))
			y += metrics.getAscent() - metrics.getHeight() / 2.0F;
		else if (textBaseline.equals("bottom")) {
			y -= metrics.getDescent();
		}
		int w = gfx.getFontMetrics().stringWidth(text);
		if ((textAlign.equals("end")) || (textAlign.equals("right")))
			x -= w;
		else if (textAlign.equals("center")) {
			x = (float) (x - 0.5D * w);
		}
		gfx.drawString(text, x, y);
		drawn = true;
	}

	public TextMetrics measureText(String text) {
		setFont();
		return new TextMetrics(gfx.getFontMetrics().stringWidth(text));
	}

	public void drawImage(HTMLImageElement image, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
		gfx.drawImage(image.getImage(), dx, dy, dx + dw, dy + dh, sx, sy, sx + sw, sy + sh, canvas.getImageObserver());
		drawn = true;
	}

	public void drawImage(HTMLCanvasElement image, int dx, int dy) {
		gfx.drawImage(image.getPixmap(), dx, dy, canvas.getImageObserver());
		drawn = true;
	}

	public void drawImage(HTMLCanvasElement image, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
		gfx.drawImage(image.getPixmap(), dx, dy, dx + dw, dy + dh, sx, sy, sx + sw, sy + sh, canvas.getImageObserver());
		drawn = true;
	}

	public void updatePixmap(BufferedImage pixmap) {
		gfx = pixmap.createGraphics();
		gfx.setBackground(new Color(0, 0, 0, 0));
		gfx.clearRect(0, 0, pixmap.getWidth(), pixmap.getHeight());

		gfx.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

		gfx.setRenderingHint(RenderingHints.KEY_STROKE_CONTROL, RenderingHints.VALUE_STROKE_PURE);
		gfx.setTransform(transform);
		gfx.setClip(clip);
		drawn = true;
	}

	public ImageData getImageData(int sx, int sy, int sw, int sh) {
		Raster data = canvas.getPixmap().getData(new Rectangle(sx, sy, sw, sh));
		return new ImageData(data);
	}

	public void putImageData(ImageData imagedata, int dx, int dy) {
		int[] data = new int[imagedata.data.length / 4];
		int p = 0;
		for (int i = 0; i < imagedata.data.length; p++) {
			data[p] = (imagedata.data[i] << 16);
			data[p] |= imagedata.data[(i + 1)] << 8;
			data[p] |= imagedata.data[(i + 2)] << 0;
			data[p] |= imagedata.data[(i + 3)] << 24;

			i += 4;
		}

		canvas.getPixmap().setRGB(dx, dy, imagedata.width, imagedata.height, data, 0, imagedata.width);
		drawn = true;
	}

	public void setTextBaseline(String textBaseline) {
		this.textBaseline = textBaseline;
		fontChanged = true;
	}

	public void setTextAlign(String textAlign) {
		this.textAlign = textAlign;
		fontChanged = true;
	}

	public void setFillStyle(Object fillStyle) {
		this.fillStyle = fillStyle;
		fillChanged = true;
	}

	public void setStrokeStyle(Object strokeStyle) {
		this.strokeStyle = strokeStyle;
		strokeChanged = true;
	}

	public void setLineWidth(float lineWidth) {
		this.lineWidth = lineWidth;
		strokeChanged = true;
	}

	public void setFont(String font) {
		this.font = font;
		fontChanged = true;
	}

	public void setShadowBlur(double shadowBlur) {
		this.shadowBlur = shadowBlur;
	}

	public void setShadowOffsetX(double shadowOffsetX) {
		this.shadowOffsetX = shadowOffsetX;
	}

	public void setShadowOffsetY(double shadowOffsetY) {
		this.shadowOffsetY = shadowOffsetY;
	}

	public void setShadowColor(Object shadowColor) {
		this.shadowColor = shadowColor;
	}

	public boolean isDrawn() {
		if (drawn) {
			drawn = false;
			return true;
		}
		return false;
	}
}